import SwiftUI

struct NotesView: View {
    @ObservedObject var manager: ClipboardManager
    @State private var showingEditSheet = false
    @State private var selectedNote: Note?
    @State private var newNoteTitle = ""
    @State private var newNoteContent = ""
    
    var body: some View {
        VStack(spacing: 0) {
            header
            
            if manager.notes.isEmpty {
                emptyState
            } else {
                notesList
            }
        }
        .background(Color(.windowBackgroundColor))
        .sheet(item: $selectedNote) { note in
            NoteEditView(note: note) { title, content in
                manager.updateNote(note, title: title, content: content)
                selectedNote = nil
            }
        }
        .sheet(isPresented: $showingEditSheet) {
            NoteEditView(note: nil) { title, content in
                manager.addNote(title: title, content: content)
                showingEditSheet = false
            }
        }
    }
    
    private var header: some View {
        HStack {
            Text("Notes")
                .font(.headline)
            Spacer()
            Button(action: {
                showingEditSheet = true
            }) {
                Image(systemName: "plus")
            }
            .buttonStyle(.plain)
        }
        .padding()
        .background(Color(.controlBackgroundColor).opacity(0.5))
    }
    
    private var notesList: some View {
        ScrollView {
            LazyVStack(spacing: 8) {
                ForEach(manager.notes) { note in
                    NoteItemView(note: note, onDelete: {
                        manager.deleteNote(note)
                    }, onEdit: {
                        selectedNote = note
                    })
                }
            }
            .padding(12)
        }
    }
    
    private var emptyState: some View {
        VStack {
            Spacer()
            Text("No notes yet")
                .foregroundColor(.secondary)
            Button("Create a note") {
                showingEditSheet = true
            }
            .padding(.top, 4)
            Spacer()
        }
    }
}

struct NoteItemView: View {
    let note: Note
    let onDelete: () -> Void
    let onEdit: () -> Void
    @State private var isHovering = false
    
    var body: some View {
        HStack {
            VStack(alignment: .leading, spacing: 4) {
                Text(note.title)
                    .font(.headline)
                    .lineLimit(1)
                Text(note.content)
                    .font(.caption)
                    .foregroundColor(.secondary)
                    .lineLimit(2)
            }
            Spacer()
            
            if isHovering {
                HStack {
                     Button(action: onEdit) {
                        Image(systemName: "pencil")
                            .foregroundColor(.blue)
                    }
                    .buttonStyle(.plain)
                    
                    Button(action: onDelete) {
                        Image(systemName: "trash")
                            .foregroundColor(.red)
                    }
                    .buttonStyle(.plain)
                }
            }
        }
        .padding(8)
        .background(isHovering ? Color.accentColor.opacity(0.1) : Color(.controlBackgroundColor).opacity(0.5))
        .cornerRadius(6)
        .onHover { hovering in
            isHovering = hovering
        }
    }
}

struct NoteEditView: View {
    @State private var title: String
    @State private var content: String
    var onSave: (String, String) -> Void
    @Environment(\.presentationMode) var presentationMode
    
    init(note: Note?, onSave: @escaping (String, String) -> Void) {
        _title = State(initialValue: note?.title ?? "")
        _content = State(initialValue: note?.content ?? "")
        self.onSave = onSave
    }
    
    var body: some View {
        VStack(spacing: 16) {
            Text("Edit Note").font(.headline)
            
            TextField("Title", text: $title)
                .textFieldStyle(.roundedBorder)
            
            TextEditor(text: $content)
                .border(Color.gray.opacity(0.2))
            
            HStack {
                Button("Cancel") {
                    presentationMode.wrappedValue.dismiss()
                }
                .keyboardShortcut(.cancelAction)
                
                Spacer()
                
                Button("Save") {
                    onSave(title, content)
                }
                .keyboardShortcut(.defaultAction)
                .disabled(title.isEmpty)
            }
        }
        .padding()
        .frame(width: 300, height: 250)
    }
}
