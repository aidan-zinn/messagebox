const CACHE = 'messagebox-v1';
const SHELL = [
  './',
  './index.html',
  './app.js',
  './style.css',
  './manifest.webmanifest',
  './creds.enc.js',
  './apple-touch-icon.png',
  './icon-192.png',
  './icon-512.png',
];

self.addEventListener('install', (e) => {
  e.waitUntil(caches.open(CACHE).then((c) => c.addAll(SHELL)).then(() => self.skipWaiting()));
});

self.addEventListener('activate', (e) => {
  e.waitUntil(
    caches.keys().then((keys) => Promise.all(keys.filter((k) => k !== CACHE).map((k) => caches.delete(k))))
      .then(() => self.clients.claim())
  );
});

self.addEventListener('fetch', (e) => {
  const url = new URL(e.request.url);
  if (url.protocol === 'wss:' || url.protocol === 'ws:' || url.hostname !== self.location.hostname) return;
  e.respondWith(
    caches.match(e.request).then((cached) => cached || fetch(e.request))
  );
});
