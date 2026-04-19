# Messagebox PWA

Static PWA that publishes messages to the Messagebox over MQTT-over-WebSockets.

## First-time setup

1. From the repo root, generate the encrypted creds file:
   ```
   node scripts/encrypt-creds.mjs
   ```
   Enter a shared password (at least 8 chars). This is the password the PWA will ask for on launch. It's *not* the MQTT password. Don't lose it — there's no recovery; just re-run the script if you forget.

2. Serve locally:
   ```
   cd web && python3 -m http.server 8080
   ```
   Open http://localhost:8080 in Safari. Enter the shared password → you should see `connected`.

3. Send a test message. The ESP32 OLED should update within a second or two.

## Icons

You need `icon-192.png`, `icon-512.png`, and `apple-touch-icon.png` (180×180) in `web/`. Any PNG works; a simple heart on a dark background is fine. The service worker will refuse to install until these files exist.

## Deploy (Cloudflare Pages)

- Create a Pages project, point it at this repo, set build output directory to `web`.
- No build command; it's static.
- After deploy, open the URL on iPhone Safari, Share → Add to Home Screen.

## Notes

- `creds.enc.js` is safe to commit — it's AES-GCM ciphertext gated by your shared password.
- Changes to `scripts/encrypt-creds.mjs` (e.g. rotating the MQTT password) require re-running the script and redeploying.
- The service worker caches the app shell. After deploying changes, bump `CACHE` in `sw.js` or do a hard reload to pick them up.
