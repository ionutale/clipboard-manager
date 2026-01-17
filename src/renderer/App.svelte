<script lang="ts">
  import { onMount } from 'svelte';

  let clipboardHistory: string[] = [];
  let searchQuery = '';

  $: filteredHistory = clipboardHistory.filter((item) =>
    item.toLowerCase().includes(searchQuery.toLowerCase())
  );

  onMount(async () => {
    // Load initial history
    clipboardHistory = await window.electronAPI.getClipboardHistory();

    // Listen for updates
    window.electronAPI.onClipboardUpdate((history) => {
      clipboardHistory = history;
    });
  });

  async function copyItem(text: string) {
    await window.electronAPI.copyToClipboard(text);
  }

  async function clearHistory() {
    if (confirm('Clear all clipboard history?')) {
      await window.electronAPI.clearHistory();
      clipboardHistory = [];
    }
  }
</script>

<main>
  <header>
    <h1>📋 Clipboard Manager</h1>
    <p class="subtitle">Cmd+Shift+V to show/hide</p>
  </header>

  <div class="controls">
    <input
      type="text"
      placeholder="Search clipboard history..."
      bind:value={searchQuery}
      class="search-input"
    />
    <button on:click={clearHistory} class="clear-btn">Clear History</button>
  </div>

  <div class="history">
    {#if filteredHistory.length === 0}
      <div class="empty-state">
        <p>No clipboard items yet</p>
        <p class="hint">Copy something to get started!</p>
      </div>
    {:else}
      {#each filteredHistory as item, index (item)}
        <button class="history-item" on:click={() => copyItem(item)}>
          <div class="item-number">{index + 1}</div>
          <div class="item-content">
            {item}
          </div>
        </button>
      {/each}
    {/if}
  </div>
</main>

<style>
  :global(body) {
    margin: 0;
    padding: 0;
    font-family:
      -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, Oxygen, Ubuntu, Cantarell, sans-serif;
    background: #1e1e1e;
    color: #e0e0e0;
  }

  main {
    padding: 20px;
    max-width: 800px;
    margin: 0 auto;
  }

  header {
    text-align: center;
    margin-bottom: 30px;
  }

  h1 {
    margin: 0;
    font-size: 2em;
    color: #fff;
  }

  .subtitle {
    margin: 5px 0 0 0;
    color: #888;
    font-size: 0.9em;
  }

  .controls {
    display: flex;
    gap: 10px;
    margin-bottom: 20px;
  }

  .search-input {
    flex: 1;
    padding: 12px;
    border: 1px solid #333;
    border-radius: 6px;
    background: #2a2a2a;
    color: #e0e0e0;
    font-size: 14px;
  }

  .search-input:focus {
    outline: none;
    border-color: #007acc;
  }

  .clear-btn {
    padding: 12px 20px;
    border: none;
    border-radius: 6px;
    background: #c42b1c;
    color: white;
    cursor: pointer;
    font-size: 14px;
    transition: background 0.2s;
  }

  .clear-btn:hover {
    background: #a52814;
  }

  .history {
    display: flex;
    flex-direction: column;
    gap: 10px;
  }

  .history-item {
    display: flex;
    gap: 12px;
    padding: 12px;
    background: #2a2a2a;
    border: 1px solid #333;
    border-radius: 6px;
    cursor: pointer;
    transition: all 0.2s;
    width: 100%;
    text-align: left;
    font-family: inherit;
    color: inherit;
    font-size: inherit;
  }

  .history-item:hover {
    background: #333;
    border-color: #007acc;
    transform: translateY(-1px);
  }

  .item-number {
    flex-shrink: 0;
    width: 30px;
    height: 30px;
    display: flex;
    align-items: center;
    justify-content: center;
    background: #007acc;
    color: white;
    border-radius: 50%;
    font-weight: bold;
    font-size: 12px;
  }

  .item-content {
    flex: 1;
    overflow: hidden;
    text-overflow: ellipsis;
    display: -webkit-box;
    -webkit-line-clamp: 3;
    -webkit-box-orient: vertical;
    line-height: 1.5;
  }

  .empty-state {
    text-align: center;
    padding: 60px 20px;
    color: #888;
  }

  .empty-state p {
    margin: 10px 0;
  }

  .empty-state .hint {
    font-size: 0.9em;
    color: #666;
  }
</style>
