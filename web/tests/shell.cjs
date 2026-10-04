// Shell integration only. The runtime is deliberately a test double, not WASM.
const assert = require('node:assert/strict');
const { chromium } = require('playwright');
(async () => {
  const browser = await chromium.launch({ executablePath: process.env.CHROMIUM || '/usr/bin/chromium', headless: true, args: ['--no-sandbox'] });
  try {
    const page = await browser.newPage();
    const errors = [];
    page.on('pageerror', error => errors.push(error.message));
    page.on('console', message => { if (message.type() === 'error') errors.push(message.text()); });
    await page.route('**/build-info.js', route => route.fulfill({contentType: 'text/javascript', body: 'window.PF_WASM_BUILT = true;'}));
    await page.route('**/dist-wasm/tetris.js', route => route.fulfill({contentType: 'text/javascript', body: `
      window.starts = 0; window.pauses = 0;
      Module._start_game = () => window.starts++;
      Module._pause_game = () => window.pauses++;
      Module.onRuntimeInitialized();
    `}));
    await page.goto(process.env.PF_WEB_URL || 'http://localhost:8000');
    await page.waitForFunction(() => !document.querySelector('#start').disabled);
    assert.equal(await page.evaluate(() => window.starts), 0);
    await page.locator('#start').click();
    assert.equal(await page.evaluate(() => window.starts), 1);
    assert.equal(await page.locator('#overlay').isVisible(), false);
    assert.equal(await page.locator('#canvas').evaluate(el => el === document.activeElement), true);
    assert.equal(await page.evaluate(() => {
      const event = new KeyboardEvent('keydown', {key: ' ', bubbles: true, cancelable: true});
      document.querySelector('#canvas').dispatchEvent(event);
      return event.defaultPrevented;
    }), true);
    await page.locator('a').focus();
    assert.equal(await page.evaluate(() => window.pauses), 1);
    assert.equal(await page.locator('a').getAttribute('href'), 'https://github.com/AliHamzaAzam/PF-Tetris/releases');
    await page.evaluate(() => window.dispatchEvent(new Event('pf-load-error')));
    assert.equal(await page.locator('#start').isDisabled(), true);
    assert.deepEqual(errors, []);
    console.log('Shell start, focus, audio key path, blur and load-failure tests passed with a runtime test double.');
  } finally { await browser.close(); }
})().catch(error => { console.error(error); process.exitCode = 1; });
