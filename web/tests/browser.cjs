const { chromium } = require('playwright');
(async () => {
  const browser = await chromium.launch({executablePath:'/usr/bin/chromium',headless:true,args:['--no-sandbox']});
  const page = await browser.newPage();
  const errors = [];
  page.on('pageerror', e => errors.push(e.message));
  page.on('console', m => {if(m.type()==='error')errors.push(m.text());});
  try {
    await page.goto(process.env.PF_WEB_URL || 'http://localhost:8000');
    await page.locator('#canvas').waitFor();
    const missing = await page.evaluate(() => !window.PF_WASM_BUILT);
    if(missing) {
      if(!await page.locator('#start').isDisabled())throw new Error('Missing build must not offer a start action');
      if(!(await page.locator('#status').textContent()).includes('not available'))throw new Error('Missing-build status is unclear');
    } else {
      await page.locator('#start').waitFor({state:'visible'});
      await page.waitForFunction(() => !document.querySelector('#start').disabled);
      await page.locator('#start').click();
      // Capture composited screenshots, not a possibly cleared WebGL buffer.
      // Paused frames freeze gravity so movement must change the actual board.
      await page.keyboard.press('p');
      await page.waitForTimeout(100);
      const before = await page.locator('#canvas').screenshot();
      await page.keyboard.press('p');
      await page.keyboard.press('ArrowRight');
      await page.keyboard.press('p');
      await page.waitForTimeout(100);
      const afterMove = await page.locator('#canvas').screenshot();
      if (before.equals(afterMove)) throw new Error('Keyboard movement did not change the paused board');
      await page.keyboard.press('p');
      await page.keyboard.press('Space');
      await page.keyboard.press('p');
      await page.waitForTimeout(100);
      const afterDrop = await page.locator('#canvas').screenshot();
      if (afterMove.equals(afterDrop)) throw new Error('Hard drop did not change the paused board');
    }
    await page.locator('#canvas').focus();
    if(!await page.locator('#canvas').evaluate(el => el===document.activeElement))throw new Error('Canvas is not focusable');
    if(errors.length)throw new Error(errors.join('\n'));
    console.log(missing?'Missing-artifact shell passed, no browser console errors. WASM gameplay unverified.':'WASM rendering and controls smoke passed, no browser console errors.');
  } finally {await browser.close();}
})().catch(e=>{console.error(e);process.exitCode=1;});
