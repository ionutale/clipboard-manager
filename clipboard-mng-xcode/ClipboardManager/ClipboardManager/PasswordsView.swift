import SwiftUI

struct PasswordsView: View {
    @ObservedObject var manager: ClipboardManager
    @State private var showingEditSheet = false
    @State private var selectedCredential: Credential?
    
    var body: some View {
        VStack(spacing: 0) {
            header
            
            if manager.credentials.isEmpty {
                emptyState
            } else {
                list
            }
        }
        .background(Color(.windowBackgroundColor))
        .sheet(item: $selectedCredential) { cred in
            CredentialEditView(credential: cred) { title, user, pass, url in
                manager.updateCredential(cred, title: title, username: user, password: pass, url: url)
                selectedCredential = nil
            }
        }
        .sheet(isPresented: $showingEditSheet) {
            CredentialEditView(credential: nil) { title, user, pass, url in
                manager.addCredential(title: title, username: user, password: pass, url: url)
                showingEditSheet = false
            }
        }
    }
    
    private var header: some View {
        HStack {
            Text("Passwords")
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
    
    private var list: some View {
        ScrollView {
            LazyVStack(spacing: 8) {
                ForEach(manager.credentials) { cred in
                    CredentialItemView(credential: cred, onDelete: {
                        manager.deleteCredential(cred)
                    }, onEdit: {
                        selectedCredential = cred
                    })
                }
            }
            .padding(12)
        }
    }
    
    private var emptyState: some View {
        VStack {
            Spacer()
            Text("No passwords yet")
                .foregroundColor(.secondary)
            Button("Add Password") {
                showingEditSheet = true
            }
            .padding(.top, 4)
            Spacer()
        }
    }
}

struct CredentialItemView: View {
    let credential: Credential
    let onDelete: () -> Void
    let onEdit: () -> Void
    @State private var isHovering = false
    
    var body: some View {
        HStack {
            Image(systemName: "key")
                .foregroundColor(.yellow)
            
            VStack(alignment: .leading, spacing: 4) {
                Text(credential.title)
                    .font(.headline)
                    .lineLimit(1)
                Text(credential.username)
                    .font(.caption)
                    .foregroundColor(.secondary)
            }
            
            Spacer()
            
            if isHovering {
                HStack(spacing: 10) {
                     Button(action: {
                        NSPasteboard.general.clearContents()
                        NSPasteboard.general.setString(credential.password, forType: .string)
                    }) {
                        Image(systemName: "doc.on.doc")
                            .foregroundColor(.blue)
                            .help("Copy Password")
                    }
                    .buttonStyle(.plain)
                    
                    Button(action: onEdit) {
                        Image(systemName: "pencil")
                            .foregroundColor(.secondary)
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

struct CredentialEditView: View {
    @State private var title: String
    @State private var username: String
    @State private var password: String
    @State private var url: String
    
    var onSave: (String, String, String, String) -> Void
    @Environment(\.presentationMode) var presentationMode
    
    init(credential: Credential?, onSave: @escaping (String, String, String, String) -> Void) {
        _title = State(initialValue: credential?.title ?? "")
        _username = State(initialValue: credential?.username ?? "")
        _password = State(initialValue: credential?.password ?? "")
        _url = State(initialValue: credential?.url ?? "")
        self.onSave = onSave
    }
    
    var body: some View {
        Form {
            Section(header: Text("Details")) {
                TextField("Title", text: $title)
                TextField("Username", text: $username)
                SecureField("Password", text: $password)
                TextField("URL", text: $url)
            }
            
            HStack {
                Button("Cancel") {
                    presentationMode.wrappedValue.dismiss()
                }
                .keyboardShortcut(.cancelAction)
                
                Spacer()
                
                Button("Save") {
                    onSave(title, username, password, url)
                }
                .keyboardShortcut(.defaultAction)
                .disabled(title.isEmpty)
            }
            .padding(.top)
        }
        .padding()
        .frame(width: 300)
    }
}
