<script>
  import { onMount } from 'svelte';
  import { GetHistory, CopyToClipboard, Quit } from '../wailsjs/go/main/App.js';
  import { EventsOn } from '../wailsjs/runtime/runtime.js';

  let history = [];
  let searchText = '';

  async function loadHistory() {
    try {
      history = (await GetHistory()) || [];
    } catch (e) {
      console.error(e);
    }
  }

  function handleCopy(item) {
    CopyToClipboard(item);
  }
  
  function quitApp() {
    Quit();
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
