<script>
  export let group;
</script>

<li>
  <span class="folder">📁 {group.name}</span>
  {#if group.groups || group.entries}
    <ul>
      {#each group.groups || [] as g}
        <svelte:self group={g} />
      {/each}
      {#each group.entries || [] as e}
        <li class="entry">🔑 <strong>{e.title}</strong>: {e.username} / ***</li>
      {/each}
    </ul>
  {/if}
</li>

<style>
  ul {
    list-style: none;
    padding-left: 20px;
  }
  .folder {
    font-weight: bold;
    cursor: default;
  }
  .entry {
    font-family: monospace;
    cursor: pointer;
  }
  .entry:hover {
    background-color: #eee;
  }
</style>
