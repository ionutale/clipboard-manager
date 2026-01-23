import SwiftUI

struct MainView: View {
    @ObservedObject var manager: ClipboardManager
    @State private var searchText = ""
    
    var filteredHistory: [ClipboardItem] {
        if searchText.isEmpty {
            return manager.history
        } else {
            return manager.history.filter { $0.content.localizedCaseInsensitiveContains(searchText) }
        }
    }
    
    var body: some View {
        VStack(spacing: 0) {
            header
            
            SearchBar(text: $searchText)
                .padding(.horizontal, 12)
                .padding(.vertical, 8)
            
            if filteredHistory.isEmpty {
                emptyState
            } else {
                historyList
            }
            
            footer
        }
        .frame(width: 300, height: 450)
        .background(Color(.windowBackgroundColor))
    }
    
    private var header: some View {
        HStack {
            Text("📋 Clipboard Manager")
                .font(.headline)
            Spacer()
            Button(action: {
                manager.clearHistory()
            }) {
                Text("Clear")
                    .font(.caption)
                    .foregroundColor(.white)
            }
            .buttonStyle(.borderedProminent)
            .tint(.red)
        }
        .padding()
        .background(Color(.controlBackgroundColor).opacity(0.5))
    }
    
    private var historyList: some View {
        ScrollView {
            LazyVStack(spacing: 8) {
                ForEach(filteredHistory) { item in
                    HistoryItemView(item: item) {
                        manager.copyToClipboard(item.content)
                    }
                }
            }
            .padding(12)
        }
    }
    
    private var emptyState: some View {
        VStack {
            Spacer()
            Text("No clipboard items yet")
                .foregroundColor(.secondary)
            Text("Copy something to get started!")
                .font(.caption)
                .foregroundColor(.secondary.opacity(0.7))
            Spacer()
        }
    }
    
    private var footer: some View {
        HStack {
            Spacer()
            Button("Quit") {
                NSApplication.shared.terminate(nil)
            }
            .font(.caption)
            .keyboardShortcut("q")
        }
        .padding(8)
        .background(Color(.controlBackgroundColor).opacity(0.3))
    }
}

struct SearchBar: View {
    @Binding var text: String
    
    var body: some View {
        HStack {
            Image(systemName: "magnifyingglass")
                .foregroundColor(.secondary)
            TextField("Search history...", text: $text)
                .textFieldStyle(.plain)
            if !text.isEmpty {
                Button(action: { text = "" }) {
                    Image(systemName: "xmark.circle.fill")
                        .foregroundColor(.secondary)
                }
                .buttonStyle(.plain)
            }
        }
        .padding(8)
        .background(Color(.controlBackgroundColor))
        .cornerRadius(8)
        .overlay(
            RoundedRectangle(cornerRadius: 8)
                .stroke(Color.secondary.opacity(0.2), lineWidth: 1)
        )
    }
}

struct HistoryItemView: View {
    let item: ClipboardItem
    let action: () -> Void
    @State private var isHovering = false
    
    var body: some View {
        Button(action: action) {
            VStack(alignment: .leading, spacing: 4) {
                Text(item.content)
                    .lineLimit(2)
                    .font(.system(size: 13))
                    .frame(maxWidth: .infinity, alignment: .leading)
                
                Text(item.timestamp.formatted(date: .omitted, time: .shortened))
                    .font(.system(size: 10))
                    .foregroundColor(.secondary)
            }
            .padding(8)
            .background(isHovering ? Color.accentColor.opacity(0.1) : Color(.controlBackgroundColor).opacity(0.5))
            .cornerRadius(6)
        }
        .buttonStyle(.plain)
        .onHover { hovering in
            isHovering = hovering
        }
    }
}
