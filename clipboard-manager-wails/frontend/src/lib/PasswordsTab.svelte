<script>
  import { UnlockKdbx } from '../../wailsjs/go/main/App.js';
  import Tree from '../Tree.svelte';

  export let initialPath = '';

  let kdbxPassword = '';
  let kdbxPath = initialPath;
  let kdbxData = null;

  // React to prop changes
  $: if (initialPath && initialPath !== kdbxPath) {
    kdbxPath = initialPath;
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

  function clear() {
    kdbxData = null;
    kdbxPassword = '';
    kdbxPath = '';
  }
</script>

<div class="tab-container">
  <div class="header">
    <h2>Passwords (KDBX)</h2>
    <div class="controls">
      <input bind:value={kdbxPath} placeholder="File path to .kdbx" />
      {#if kdbxData}
        <button on:click={clear}>Lock / Close</button>
      {/if}
    </div>
  </div>

  <div class="content-area">
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
      {#if !kdbxPath}
        <p class="hint">Select a .kdbx file from clipboard or enter path above.</p>
      {/if}
    {:else}
      <div class="tree-view">
        <ul><Tree group={kdbxData} /></ul>
      </div>
    {/if}
  </div>
</div>

<style>
  .tab-container {
    display: flex;
    flex-direction: column;
    height: 100%;
    padding: 10px;
    box-sizing: border-box;
  }

  .header {
    margin-bottom: 20px;
  }

  .controls {
    display: flex;
    gap: 10px;
  }

  h2 {
    margin: 0 0 10px 0;
    font-size: 1.2rem;
  }

  input {
    flex: 1;
    padding: 8px;
    border: 1px solid #ddd;
    border-radius: 4px;
  }

  .content-area {
    flex: 1;
    display: flex;
    flex-direction: column;
    justify-content: center;
    overflow: hidden;
  }

  .auth-box {
    display: flex;
    gap: 10px;
    justify-content: center;
    align-items: center;
  }

  .tree-view {
    flex: 1;
    overflow-y: auto;
    text-align: left;
    border: 1px solid #eee;
    padding: 10px;
    background: white;
    border-radius: 4px;
  }

  .hint {
    text-align: center;
    color: #777;
    margin-top: 20px;
  }
</style>
