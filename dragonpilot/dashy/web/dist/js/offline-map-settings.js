(() => {
  'use strict';

  const messages = {
    not_installed: '所選地區尚未下載',
    unchecked: '尚未檢查更新',
    checking: '正在檢查…',
    available: '有新圖資可更新',
    current: '已是最新圖資',
    unknown: '無法確認是否有新版，請稍後再試',
    cancelled: '檢查已中止',
  };

  function setOptions(select, options, value, emptyLabel) {
    if (!select.options.length) {
      if (emptyLabel) select.add(new Option(emptyLabel, ''));
      const entries = Object.entries(options).sort((a, b) => a[1].localeCompare(b[1]));
      for (const [code, name] of entries) select.add(new Option(name, code));
    }
    select.value = value;
  }

  function render(container) {
    const panel = document.createElement('div');
    panel.className = 'p-4 space-y-3';
    panel.innerHTML = `
      <label class="block text-sm">國家
        <select class="select select-bordered w-full" name="country" disabled></select>
      </label>
      <label class="text-sm" data-state hidden>美國州別
        <select class="select select-bordered w-full" name="state" disabled></select>
      </label>
      <label class="flex items-center gap-2 text-sm">
        <input type="checkbox" class="toggle" name="enabled" disabled>
        車機與 Dashy 道路限速與名稱
      </label>
      <p class="text-sm">下載狀態：<span data-download>正在讀取圖資狀態…</span></p>
      <p class="text-sm" role="status"></p>
      <div class="flex flex-wrap gap-2">
        <button type="button" class="btn btn-primary btn-sm" value="download" disabled>下載／更新圖資</button>
        <button type="button" class="btn btn-outline btn-sm" value="check_update" disabled>檢查新版</button>
        <button type="button" class="btn btn-outline btn-error btn-sm" value="delete" disabled>刪除所有離線圖資</button>
      </div>
      <p class="text-sm text-base-content/60" data-feedback></p>`;
    container.appendChild(panel);
    const country = panel.querySelector('[name="country"]');
    const state = panel.querySelector('[name="state"]');
    const enabled = panel.querySelector('[name="enabled"]');
    const status = panel.querySelector('[role="status"]');
    const download = panel.querySelector('[data-download]');
    const feedback = panel.querySelector('[data-feedback]');
    const buttons = [...panel.querySelectorAll('button')];
    let data;
    let timer;
    let requestId = 0;

    function display() {
      const available = data.offroad && !data.busy;
      setOptions(country, data.countries, data.country);
      setOptions(state, data.states, data.state, '全美國（大型下載）');
      panel.querySelector('[data-state]').hidden = data.country !== 'US';
      country.disabled = !available;
      state.disabled = !available;
      enabled.disabled = !available;
      enabled.checked = data.enabled;
      download.textContent = data.downloadStatus;
      status.textContent = `圖資更新：${messages[data.status] || messages.unknown}`;
      for (const button of buttons) {
        button.disabled = !available || (button.value === 'check_update' && !data.installed);
      }
      feedback.textContent = !data.offroad ? '停車後可變更設定或管理圖資。' :
        data.busy ? '車機正在處理圖資，請稍候。' : '';
    }

    async function refresh(action, value) {
      clearTimeout(timer);
      const id = ++requestId;
      if (action) {
        for (const control of [country, state, enabled, ...buttons]) control.disabled = true;
      }
      try {
        const options = {cache: 'no-store', signal: AbortSignal.timeout(5000)};
        if (action) {
          options.method = 'POST';
          options.headers = {'Content-Type': 'application/json'};
          options.body = JSON.stringify({action, value, key: data.key});
        }
        const response = await fetch('/api/settings/map-update', options);
        const result = await response.json();
        if (!panel.isConnected || id !== requestId) return;
        if (!result.countries) throw new Error('Missing offline map settings');
        data = result;
        display();
        if (!response.ok) feedback.textContent = result.error === 'region_changed'
          ? '所選地區已變更，請確認後重試。' : '目前無法執行此操作。';
      } catch {
        if (id !== requestId) return;
        for (const control of [country, state, enabled, ...buttons]) control.disabled = true;
        feedback.textContent = '無法讀取圖資設定，正在重試。';
      } finally {
        if (panel.isConnected && id === requestId) timer = setTimeout(() => refresh(), 2000);
      }
    }

    for (const select of [country, state]) {
      select.addEventListener('change', () => refresh(select.name, select.value));
    }
    enabled.addEventListener('change', () => refresh('enabled', enabled.checked));
    for (const button of buttons) {
      button.addEventListener('click', () => {
        if (button.value === 'download' && !window.confirm(
          `下載 ${country.selectedOptions[0].text}${data.country === 'US' ? `／${state.selectedOptions[0].text}` : ''} 的離線圖資？可能需要大量空間與時間。`
        )) return;
        if (button.value === 'delete' && !window.confirm('刪除所有已下載的離線道路圖資？')) return;
        refresh(button.value);
      });
    }
    refresh();
  }

  window.DashyOfflineMapSettings = {render};
})();
