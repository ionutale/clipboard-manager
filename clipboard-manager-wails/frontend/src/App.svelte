<script>
  import ClipboardTab from './lib/ClipboardTab.svelte';
  import PasswordsTab from './lib/PasswordsTab.svelte';
  import NotesTab from './lib/NotesTab.svelte';
  import { Quit } from '../wailsjs/go/main/App.js';

  let activeTab = 'clipboard'; // 'clipboard' | 'passwords' | 'notes'
  let passwordInitialPath = '';

  function switchTab(tab) {
    activeTab = tab;
  }

  function handleOpenKdbx(path) {
    passwordInitialPath = path;
    activeTab = 'passwords';
  }

  function quitApp() {
    Quit();
  }
</script>

<main>
  <nav class="sidebar-nav">
    <div
      class="nav-item {activeTab === 'clipboard' ? 'active' : ''}"
      on:click={() => switchTab('clipboard')}
      title="Clipboard"
    >
      <span class="icon">📋</span>
    </div>
    <div
      class="nav-item {activeTab === 'passwords' ? 'active' : ''}"
      on:click={() => switchTab('passwords')}
      title="Passwords"
    >
      <span class="icon">🔐</span>
    </div>
    <div
      class="nav-item {activeTab === 'notes' ? 'active' : ''}"
      on:click={() => switchTab('notes')}
      title="Notes"
    >
      <span class="icon">📝</span>
    </div>
    <div class="spacer"></div>
    <div class="nav-item quit" on:click={quitApp} title="Quit App">
      <span class="icon">❌</span>
    </div>
  </nav>

  <div class="content">
    <div class="tab-content" style="display: {activeTab === 'clipboard' ? 'block' : 'none'}">
      <ClipboardTab onOpenKdbx={handleOpenKdbx} />
    </div>
    <div class="tab-content" style="display: {activeTab === 'passwords' ? 'block' : 'none'}">
      <PasswordsTab initialPath={passwordInitialPath} />
    </div>
    <div class="tab-content" style="display: {activeTab === 'notes' ? 'block' : 'none'}">
      <NotesTab />
    </div>
  </div>
</main>

<style>
  main {
    height: 100vh;
    display: flex;
    font-family:
      -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, Oxygen, Ubuntu, Cantarell, 'Open Sans',
      'Helvetica Neue', sans-serif;
    overflow: hidden;
  }

  .sidebar-nav {
    width: 60px;
    background-color: #2d3748;
    display: flex;
    flex-direction: column;
    align-items: center;
    padding-top: 20px;
    color: white;
  }

  .nav-item {
    width: 40px;
    height: 40px;
    display: flex;
    justify-content: center;
    align-items: center;
    margin-bottom: 15px;
    border-radius: 8px;
    cursor: pointer;
    transition: background 0.2s;
    font-size: 1.5rem;
    opacity: 0.6;
  }

  .nav-item:hover {
    background-color: rgba(255, 255, 255, 0.1);
    opacity: 1;
  }

  .nav-item.active {
    background-color: #4299e1;
    opacity: 1;
    box-shadow: 0 4px 6px rgba(0, 0, 0, 0.1);
  }

  .spacer {
    flex: 1;
  }

  .quit {
    margin-bottom: 20px;
    background-color: #e53e3e;
    opacity: 1;
  }
  .quit:hover {
    background-color: #c53030;
  }

  .content {
    flex: 1;
    background-color: #f7fafc;
    overflow: hidden;
    position: relative;
  }

  .tab-content {
    height: 100%;
    width: 100%;
  }
</style>
