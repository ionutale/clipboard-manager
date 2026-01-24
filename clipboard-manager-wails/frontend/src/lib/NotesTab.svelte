<script>
  import { onMount } from 'svelte';
  import { GetNotes, CreateNote, UpdateNote, DeleteNote } from '../../wailsjs/go/main/App.js';

  let notes = [];
  let selectedNote = null;
  let editTitle = '';
  let editContent = '';
  let searchText = '';

  async function loadNotes() {
    try {
      notes = (await GetNotes()) || [];
    } catch (e) {
      console.error(e);
    }
  }

  function selectNote(note) {
    selectedNote = note;
    editTitle = note.title;
    editContent = note.content;
  }

  function newNote() {
    selectedNote = { id: '', title: '', content: '' };
    editTitle = '';
    editContent = '';
  }

  async function saveNote() {
    if (!editTitle) return;
    try {
      if (selectedNote.id) {
        await UpdateNote(selectedNote.id, editTitle, editContent);
      } else {
        await CreateNote(editTitle, editContent);
      }
      await loadNotes();
      // Keep selection or find new note logic? For simplicity, deselect or re-select.
      // Re-selecting the just updated note would require finding it in the new list.
      const updated = notes.find((n) => n.title === editTitle && n.content === editContent); // approx
      if (updated) selectNote(updated);
      else selectedNote = null;
    } catch (e) {
      console.error(e);
      alert('Error saving note: ' + e);
    }
  }

  async function deleteNote(id) {
    if (!confirm('Delete note?')) return;
    try {
      await DeleteNote(id);
      await loadNotes();
      selectedNote = null;
    } catch (e) {
      console.error(e);
    }
  }

  onMount(() => {
    loadNotes();
  });

  $: filteredNotes = notes.filter(
    (n) =>
      n.title.toLowerCase().includes(searchText.toLowerCase()) ||
      n.content.toLowerCase().includes(searchText.toLowerCase())
  );
</script>

<div class="tab-container">
  <div class="sidebar">
    <div class="sidebar-header">
      <input bind:value={searchText} placeholder="Search notes..." class="search" />
      <button on:click={newNote} class="new-btn">+</button>
    </div>
    <div class="note-list">
      {#each filteredNotes as note}
        <div
          class="note-item {selectedNote && selectedNote.id === note.id ? 'active' : ''}"
          on:click={() => selectNote(note)}
        >
          <div class="note-title">{note.title}</div>
          <div class="note-preview">{note.content.substring(0, 30)}...</div>
          <div class="note-date">{new Date(note.createdAt).toLocaleDateString()}</div>
        </div>
      {/each}
    </div>
  </div>

  <div class="main-editor">
    {#if selectedNote}
      <div class="editor-toolbar">
        <input bind:value={editTitle} placeholder="Note Title" class="title-input" />
        <div class="actions">
          <button on:click={saveNote} class="save-btn">Save</button>
          {#if selectedNote.id}
            <button on:click={() => deleteNote(selectedNote.id)} class="delete-btn">Delete</button>
          {/if}
        </div>
      </div>
      <textarea bind:value={editContent} placeholder="Write something..." class="content-input"
      ></textarea>
    {:else}
      <div class="empty-state">
        <p>Select a note or create a new one.</p>
        <button on:click={newNote}>Create New Note</button>
      </div>
    {/if}
  </div>
</div>

<style>
  .tab-container {
    display: flex;
    height: 100%;
    background: white;
  }

  .sidebar {
    width: 250px;
    border-right: 1px solid #ddd;
    display: flex;
    flex-direction: column;
    background: #f9f9f9;
  }

  .sidebar-header {
    padding: 10px;
    display: flex;
    gap: 5px;
  }

  .search {
    flex: 1;
    padding: 5px;
  }

  .note-list {
    flex: 1;
    overflow-y: auto;
  }

  .note-item {
    padding: 10px;
    border-bottom: 1px solid #eee;
    cursor: pointer;
  }
  .note-item:hover {
    background: #eee;
  }
  .note-item.active {
    background: #e0e7ff;
  }

  .note-title {
    font-weight: bold;
    font-size: 0.9em;
    margin-bottom: 3px;
  }
  .note-preview {
    font-size: 0.8em;
    color: #666;
    margin-bottom: 3px;
  }
  .note-date {
    font-size: 0.7em;
    color: #999;
  }

  .main-editor {
    flex: 1;
    display: flex;
    flex-direction: column;
    padding: 20px;
  }

  .editor-toolbar {
    display: flex;
    justify-content: space-between;
    margin-bottom: 20px;
  }

  .title-input {
    font-size: 1.5em;
    border: none;
    outline: none;
    width: 100%;
    font-weight: bold;
  }

  .content-input {
    flex: 1;
    border: none;
    outline: none;
    resize: none;
    font-family: inherit;
    font-size: 1em;
    line-height: 1.5;
  }

  .empty-state {
    flex: 1;
    display: flex;
    flex-direction: column;
    justify-content: center;
    align-items: center;
    color: #888;
  }

  button {
    cursor: pointer;
    padding: 5px 10px;
    border: 1px solid #ccc;
    border-radius: 4px;
    background: white;
  }
  .save-btn {
    background: #4f46e5;
    color: white;
    border-color: #4f46e5;
  }
  .delete-btn {
    color: #ef4444;
    border-color: #ef4444;
    margin-left: 5px;
  }
</style>
