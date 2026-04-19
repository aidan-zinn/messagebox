import mqtt from 'https://esm.sh/mqtt@5.10.1';
import { ENC_CREDS } from './creds.enc.js';

const TOPIC = 'messagebox/message';
const HISTORY_KEY = 'messagebox.history';
const SESSION_KEY = 'messagebox.creds';
const MAX_HISTORY = 20;

const $ = (id) => document.getElementById(id);

const b64 = {
  toBytes: (s) => Uint8Array.from(atob(s), (c) => c.charCodeAt(0)),
};

async function deriveKey(password, saltB64) {
  const km = await crypto.subtle.importKey(
    'raw', new TextEncoder().encode(password), 'PBKDF2', false, ['deriveKey']
  );
  return crypto.subtle.deriveKey(
    { name: 'PBKDF2', salt: b64.toBytes(saltB64), iterations: 200000, hash: 'SHA-256' },
    km, { name: 'AES-GCM', length: 256 }, false, ['decrypt']
  );
}

async function decryptCreds(password) {
  const key = await deriveKey(password, ENC_CREDS.salt);
  const pt = await crypto.subtle.decrypt(
    { name: 'AES-GCM', iv: b64.toBytes(ENC_CREDS.iv) },
    key, b64.toBytes(ENC_CREDS.ct)
  );
  return JSON.parse(new TextDecoder().decode(pt));
}

let client = null;
let connected = false;

function setStatus(text, cls = '') {
  const el = $('status');
  el.textContent = text;
  el.className = 'status ' + cls;
  $('send-btn').disabled = !connected;
}

function connect(creds) {
  const url = `wss://${creds.host}:8884/mqtt`;
  client = mqtt.connect(url, {
    username: creds.username,
    password: creds.password,
    clientId: 'messagebox-web-' + Math.random().toString(36).slice(2, 10),
    clean: true,
    reconnectPeriod: 3000,
    connectTimeout: 10000,
  });

  client.on('connect', () => { connected = true; setStatus('connected', 'connected'); });
  client.on('reconnect', () => setStatus('reconnecting…'));
  client.on('close', () => { connected = false; setStatus('disconnected'); });
  client.on('error', (err) => {
    console.error('mqtt error', err);
    setStatus('error', 'error');
  });
}

function publish(text) {
  return new Promise((resolve, reject) => {
    client.publish(TOPIC, text, { qos: 1, retain: true }, (err) => err ? reject(err) : resolve());
  });
}

function loadHistory() {
  try { return JSON.parse(localStorage.getItem(HISTORY_KEY) || '[]'); }
  catch { return []; }
}

function saveHistory(list) {
  localStorage.setItem(HISTORY_KEY, JSON.stringify(list.slice(0, MAX_HISTORY)));
}

function renderHistory() {
  const list = loadHistory();
  const ul = $('history');
  ul.innerHTML = '';
  for (const item of list) {
    const li = document.createElement('li');
    const ts = document.createElement('span');
    ts.className = 'ts';
    ts.textContent = new Date(item.ts).toLocaleString();
    const body = document.createElement('span');
    body.className = 'body';
    body.textContent = item.text;
    li.append(ts, body);
    ul.append(li);
  }
}

function pushHistory(text) {
  const list = loadHistory();
  list.unshift({ ts: Date.now(), text });
  saveHistory(list);
  renderHistory();
}

async function unlock(password) {
  const creds = await decryptCreds(password);
  sessionStorage.setItem(SESSION_KEY, JSON.stringify(creds));
  return creds;
}

function showCompose() {
  $('unlock-view').hidden = true;
  $('compose-view').hidden = false;
  renderHistory();
}

$('unlock-form').addEventListener('submit', async (e) => {
  e.preventDefault();
  $('unlock-error').hidden = true;
  try {
    const creds = await unlock($('password').value);
    showCompose();
    setStatus('connecting…');
    connect(creds);
  } catch (err) {
    console.error(err);
    $('unlock-error').hidden = false;
  }
});

$('message').addEventListener('input', (e) => {
  $('counter').textContent = `${e.target.value.length} / 240`;
});

$('send-form').addEventListener('submit', async (e) => {
  e.preventDefault();
  const text = $('message').value.trim();
  if (!text || !connected) return;
  $('send-btn').disabled = true;
  setStatus('sending…');
  try {
    await publish(text);
    pushHistory(text);
    $('message').value = '';
    $('counter').textContent = '0 / 240';
    setStatus('sent ✓', 'connected');
  } catch (err) {
    console.error(err);
    setStatus('send failed', 'error');
  } finally {
    $('send-btn').disabled = !connected;
  }
});

const cached = sessionStorage.getItem(SESSION_KEY);
if (cached) {
  try {
    const creds = JSON.parse(cached);
    showCompose();
    setStatus('connecting…');
    connect(creds);
  } catch {
    sessionStorage.removeItem(SESSION_KEY);
  }
}

if ('serviceWorker' in navigator) {
  navigator.serviceWorker.register('./sw.js').catch(console.warn);
}
