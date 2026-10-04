'use strict';
const canvas = document.getElementById('canvas');
const start = document.getElementById('start');
const status = document.getElementById('status');
let audio;
function failed(message) {
  status.textContent = message;
  start.textContent = 'Build unavailable';
  start.disabled = true;
  document.querySelector('#overlay h2').textContent = 'Browser build unavailable';
  document.querySelector('#overlay p').textContent = 'You can still download a native release below.';
}
window.Module = {
  canvas,
  locateFile: name => `dist-wasm/${name}`,
  onRuntimeInitialized() {
    start.disabled = false;
    start.textContent = 'Start game';
    status.textContent = 'Ready. Click Start game to play.';
  },
  onAbort() { failed('The game could not load. Please reload or try a native release.'); }
};
window.addEventListener('pf-load-error', () => failed('A graphics resource could not load. Please rebuild the browser files.'));
start.addEventListener('click', () => {
  const AudioContext = window.AudioContext || window.webkitAudioContext;
  if (AudioContext) {
    try { audio = new AudioContext(); audio.resume().catch(() => {}); } catch (_) { /* Play silently when unavailable. */ }
  }
  Module._start_game();
  document.getElementById('overlay').hidden = true;
  document.getElementById('overlay').style.display = 'none';
  status.textContent = 'Playing. Press P to pause.';
  canvas.focus();
});
canvas.addEventListener('pointerdown', () => canvas.focus());
canvas.addEventListener('keydown', event => {
  if (['ArrowLeft','ArrowRight','ArrowUp','ArrowDown',' ','Enter','Escape','p','P'].includes(event.key)) event.preventDefault();
  if (event.key === ' ' && audio && audio.state === 'running') {
    const tone = audio.createOscillator(); const volume = audio.createGain();
    tone.frequency.value = 180; volume.gain.setValueAtTime(.03, audio.currentTime);
    volume.gain.exponentialRampToValueAtTime(.001, audio.currentTime + .06);
    tone.connect(volume); volume.connect(audio.destination); tone.start(); tone.stop(audio.currentTime + .06);
  }
});
canvas.addEventListener('blur', () => { if (Module._pause_game) Module._pause_game(); });
document.addEventListener('visibilitychange', () => { if (document.hidden && Module._pause_game) Module._pause_game(); });
if (window.PF_WASM_BUILT) {
  const script = document.createElement('script'); script.src = 'dist-wasm/tetris.js';
  script.onerror = () => failed('The browser build is incomplete. Please rebuild or try a native release.');
  document.body.appendChild(script);
} else {
  failed('The browser game is not available yet. Download a native release below.');
}
