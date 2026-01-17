<script lang="ts">
  import { onMount } from 'svelte';

  interface ClipboardItem {
    content: string;
    timestamp: number;
  }

  let clipboardHistory: ClipboardItem[] = [];
  let searchQuery = '';

  $: filteredHistory = clipboardHistory.filter((item) =>
    item.content.toLowerCase().includes(searchQuery.toLowerCase())
  );

  onMount(async () => {
    console.log('[Renderer] Component mounted, electronAPI available:', !!window.electronAPI);

    try {
      // Load initial history
      clipboardHistory = await window.electronAPI.getClipboardHistory();
      console.log('[Renderer] Initial history loaded:', clipboardHistory);
    } catch (error) {
      console.error('[Renderer] Error loading history:', error);
    }

    // Listen for updates
    window.electronAPI.onClipboardUpdate((history) => {
      console.log('[Renderer] Received clipboard update:', history);
      clipboardHistory = history;
    });
  });

  async function copyItem(text: string) {
    await window.electronAPI.copyToClipboard(text);
  }

  function formatTime(timestamp: number) {
    // Format timestamp to 24h format
    const date = new Date(timestamp);
    return date.toLocaleTimeString([], { hour: '2-digit', minute: '2-digit', hour12: false });
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
      {#each filteredHistory as item, index (item.timestamp)}
        <button class="history-item" on:click={() => copyItem(item.content)}>
          <div class="item-number">{index + 1}</div>
          <div class="item-details">
            <div class="item-content">
              {item.content}
            </div>
            <div class="item-meta">
              {formatTime(item.timestamp)}
            </div>
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
    background: transparent; /* Transparent for frameless window effect */
    color: #e0e0e0;
    overflow: hidden;
  }

  main {
    padding: 12px;
    height: 100vh; /* Full height */
    box-sizing: border-box;
    background: #1e1e1e; /* Solid background for content */
    display: flex;
    flex-direction: column;
    border: 1px solid #444; /* Subtle border */
    border-radius: 8px; /* Rounded corners */
  }

  header {
    text-align: center;
    margin-bottom: 16px;
    -webkit-app-region: drag; /* Allow window dragging */
    cursor: default;
  }

  h1 {
    margin: 0;
    font-size: 1.4em;
    color: #fff;
  }

  .subtitle {
    margin: 2px 0 0 0;
    color: #888;
    font-size: 0.8em;
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
    overflow-y: auto;
    flex: 1;
    padding-right: 4px;
  }

  /* Scrollbar Styling */
  .history::-webkit-scrollbar {
    width: 6px;
  }
  .history::-webkit-scrollbar-track {
    background: transparent;
  }
  .history::-webkit-scrollbar-thumb {
    background: #444;
    border-radius: 3px;
  }
  .history::-webkit-scrollbar-thumb:hover {
    background: #555;
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
    width: 24px;
    height: 24px;
    display: flex;
    align-items: center;
    justify-content: center;
    background: #007acc;
    color: white;
    border-radius: 50%;
    font-weight: bold;
    font-size: 11px;
    margin-top: 2px;
  }

  .item-details {
    flex: 1;
    overflow: hidden;
    display: flex;
    flex-direction: column;
    gap: 4px;
  }

  .item-content {
    overflow: hidden;
    text-overflow: ellipsis;
    display: -webkit-box;
    -webkit-line-clamp: 2;
    -webkit-box-orient: vertical;
    line-height: 1.4;
    font-size: 13px;
    color: #e0e0e0;
  }

  .item-meta {
    font-size: 11px;
    color: #888;
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
