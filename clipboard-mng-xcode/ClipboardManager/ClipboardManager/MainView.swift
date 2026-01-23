import SwiftUI

struct MainView: View {
    @ObservedObject var manager: ClipboardManager
    @State private var searchText = ""
    
    var filteredHistory: [ClipboardItem] {
        if searchText.isEmpty {
            return manager.history
        } else {
            return manager.history.filter { item in
                switch item.type {
                case .text:
                    return item.content.localizedCaseInsensitiveContains(searchText)
                case .file:
                    return item.content.localizedCaseInsensitiveContains(searchText)
                case .image:
                    return false
                }
            }
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
                        manager.copyToClipboard(item)
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
            HStack(spacing: 8) {
                // Type Icon
                Group {
                    switch item.type {
                    case .text:
                        Image(systemName: "text.alignleft")
                            .foregroundColor(.blue)
                    case .image:
                        Image(systemName: "photo")
                            .foregroundColor(.purple)
                    case .file:
                        Image(systemName: "doc")
                            .foregroundColor(.orange)
                    }
                }
                .font(.system(size: 16))
                .frame(width: 24)
                
                VStack(alignment: .leading, spacing: 4) {
                    contentView
                    
                    Text(item.timestamp.formatted(date: .omitted, time: .shortened))
                        .font(.system(size: 10))
                        .foregroundColor(.secondary)
                }
                .frame(maxWidth: .infinity, alignment: .leading)
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
    
    @ViewBuilder
    var contentView: some View {
        switch item.type {
        case .text:
            Text(item.content)
                .lineLimit(2)
                .font(.system(size: 13))
        case .image:
            if let image = NSImage(contentsOfFile: item.content) {
                Image(nsImage: image)
                    .resizable()
                    .aspectRatio(contentMode: .fit)
                    .frame(height: 50)
                    .cornerRadius(4)
            } else {
                Text("Image missing")
                    .italic()
                    .font(.caption)
            }
        case .file:
            Text(URL(fileURLWithPath: item.content).lastPathComponent)
                .lineLimit(1)
                .font(.system(size: 13))
        }
    }
}
