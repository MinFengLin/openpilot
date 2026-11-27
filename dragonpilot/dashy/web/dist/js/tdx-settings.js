/* Device credential operations. Existing credentials are never loaded into the page. */
(() => {
  'use strict';
  const messages = {
    saved: '已儲存到車機。',
    verified: '金鑰正確，TDX 已核發存取權杖。',
    invalid_input: '請完整填寫 Client ID 和 Client Secret。',
    offroad_required: '請停車並讓車機進入停車模式後操作。',
    rejected: 'TDX 未核發權杖。請確認金鑰與啟用狀態。',
    rate_limited: 'TDX 暫時限制請求，請稍後再試。',
    upstream_failed: '無法完成 TDX 驗證，請稍後再試。',
    save_failed: '車機儲存金鑰失敗。',
  };

  function render(container) {
    const form = document.createElement('form');
    form.className = 'p-4 space-y-3';
    form.autocomplete = 'off';
    form.innerHTML = `
      <h2 class="font-semibold">TDX API 金鑰</h2>
      <p class="text-sm text-base-content/60">停車時可儲存或驗證。兩欄留空時，驗證車機已儲存的金鑰。</p>
      <label class="block text-sm">Client ID
        <input class="input input-bordered w-full" type="text" maxlength="255" autocomplete="off" spellcheck="false">
      </label>
      <label class="block text-sm">Client Secret
        <input class="input input-bordered w-full" type="password" maxlength="255" autocomplete="new-password">
      </label>
      <div class="flex flex-wrap gap-2">
        <button type="submit" class="btn btn-outline btn-sm" value="check">驗證金鑰</button>
        <button type="submit" class="btn btn-primary btn-sm" value="save">儲存金鑰</button>
      </div>
      <p class="text-sm" role="status" hidden></p>`;
    const [clientId, clientSecret] = form.querySelectorAll('input');
    const buttons = form.querySelectorAll('button');
    const status = form.querySelector('[role="status"]');
    form.addEventListener('submit', async event => {
      event.preventDefault();
      const operation = event.submitter?.value || 'save';
      buttons.forEach(button => { button.disabled = true; });
      status.hidden = false;
      status.textContent = operation === 'check' ? '正在向 TDX 驗證…' : '正在儲存…';
      try {
        const response = await fetch(`/api/settings/tdx/${operation}`, {
          method: 'POST',
          headers: {'Content-Type': 'application/json'},
          cache: 'no-store',
          body: JSON.stringify({clientId: clientId.value, clientSecret: clientSecret.value}),
        });
        const result = await response.json();
        status.textContent = messages[result.status] || '操作失敗。';
        if (result.status === 'saved') form.reset();
      } catch {
        status.textContent = '無法連線到車機。';
      } finally {
        buttons.forEach(button => { button.disabled = false; });
      }
    });
    container.appendChild(form);
  }

  window.DashyTdxSettings = {render};
})();
