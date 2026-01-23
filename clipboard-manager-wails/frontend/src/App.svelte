<script>
  import { onMount } from 'svelte';
  import { GetHistory, CopyToClipboard, Quit, UnlockKdbx } from '../wailsjs/go/main/App.js';
  import { EventsOn } from '../wailsjs/runtime/runtime.js';
  import Tree from './Tree.svelte';

  let history = [];
  let searchText = '';
  let showKdbxModal = false;
  let kdbxPassword = '';
  let kdbxPath = '';
  let kdbxData = null;

  async function loadHistory() {
    try {
      history = (await GetHistory()) || [];
    } catch (e) {
      console.error(e);
    }
  }

  function handleCopy(item) {
    if (item.type === 'kdbx') {
      openKdbx(item);
    } else {
      CopyToClipboard(item);
    }
  }

  function quitApp() {
    Quit();
  }

  function openKdbx(item) {
    kdbxPath = item.content;
    showKdbxModal = true;
    kdbxData = null;
    kdbxPassword = '';
  }

  async function unlock() {
    try {
      kdbxData = await UnlockKdbx(kdbxPath, kdbxPassword);
    } catch (e) {
      alert('Failed to unlock: ' + e);
    }
  }

  function closeKdbx() {
    showKdbxModal = false;
  }

  onMount(() => {
    loadHistory();

    // Listen for new items from backend
    EventsOn('clipboard-new-item', (item) => {
      // Deduplicate
      history = history.filter((h) => h.content !== item.content);
      history = [item, ...history];
    });
  });

  $: filteredHistory = searchText
    ? history.filter((h) => h.content.toLowerCase().includes(searchText.toLowerCase()))
    : history;
</script>

<main>
  <div class="header">
    <input bind:value={searchText} placeholder="Search history..." />
    <button on:click={quitApp} class="quit-btn">Quit</button>
  </div>

  <div class="list">
    {#each filteredHistory as item}
      <button class="item" on:click={() => handleCopy(item)}>
        <div class="type-icon">
          {#if item.type === 'text'}
            📝
          {:else if item.type === 'kdbx'}
            🔐
          {:else if item.type === 'image'}
            🖼️
          {:else}
            📁
          {/if}
        </div>
        <div class="content">
          {#if item.type === 'image'}
            <img src={item.content} alt="clipboard content" height="50" />
          {:else}
            {item.content}
          {/if}
        </div>
      </button>
    {/each}
  </div>
</main>

{#if showKdbxModal}
  <div class="modal-overlay">
    <div class="modal">
      <h3>Unlock KDBX</h3>
      <p class="path">{kdbxPath}</p>
      {#if !kdbxData}
        <div class="auth-box">
          <input
            type="password"
            bind:value={kdbxPassword}
            placeholder="Master Password"
            on:keydown={(e) => e.key === 'Enter' && unlock()}
          />
          <button on:click={unlock}>Unlock</button>
        </div>
      {:else}
        <div class="tree-view">
          <ul><Tree group={kdbxData} /></ul>
        </div>
      {/if}
      <button on:click={closeKdbx} style="margin-top: 10px; align-self: flex-end;">Close</button>
    </div>
  </div>
{/if}

<style>
  main {
    height: 100vh;
    display: flex;
    flex-direction: column;
    padding: 10px;
    background-color: #f5f5f5;
    font-family: sans-serif;
  }

  .header {
    margin-bottom: 10px;
  }

  input {
    width: 100%;
    padding: 8px;
    border: 1px solid #ddd;
    border-radius: 4px;
  }

  .list {
    flex: 1;
    overflow-y: auto;
    display: flex;
    flex-direction: column;
    gap: 5px;
  }

  .item {
    display: flex;
    align-items: center;
    padding: 8px;
    background: white;
    border: 1px solid #ddd;
    border-radius: 4px;
    cursor: pointer;
    text-align: left;
  }

  .item:hover {
    background: #eee;
  }

  .type-icon {
    margin-right: 10px;
    font-size: 1.2em;
  }

  /* Modal Styles */
  .modal-overlay {
    position: fixed;
    top: 0;
    left: 0;
    right: 0;
    bottom: 0;
    background: rgba(0, 0, 0, 0.5);
    display: flex;
    justify-content: center;
    align-items: center;
    z-index: 100;
  }
  .modal {
    background: white;
    padding: 20px;
    border-radius: 8px;
    width: 90%;
    max-width: 600px;
    max-height: 80vh;
    overflow: auto;
    display: flex;
    flex-direction: column;
    box-shadow: 0 4px 6px rgba(0, 0, 0, 0.1);
  }
  .path {
    font-size: 0.8em;
    color: #666;
    margin-bottom: 20px;
    word-break: break-all;
  }
  .auth-box {
    display: flex;
    gap: 10px;
  }
  .tree-view {
    overflow-y: auto;
    text-align: left;
    border: 1px solid #eee;
    padding: 10px;
    margin-bottom: 10px;
  }

  .content {
    overflow: hidden;
    white-space: nowrap;
    text-overflow: ellipsis;
    flex: 1;
    font-size: 0.9em;
  }

  img {
    max-width: 100%;
    object-fit: contain;
  }
</style>
