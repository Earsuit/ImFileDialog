#pragma once

#include <memory>
#include <ctime>
#include <stack>
#include <string>
#include <thread>
#include <vector>
#include <functional>
#include <filesystem>
#include <unordered_map>
#include <algorithm> // std::min, std::max

#define IFD_DIALOG_FILE			0
#define IFD_DIALOG_DIRECTORY	1
#define IFD_DIALOG_SAVE			2

namespace ifd {
	enum class Format: char{
		BGRA,
		RGBA,
		RGB
	};

	class FileDialog {
	public:
		static inline FileDialog& getInstance()
		{
			static FileDialog ret;
			return ret;
		}

		~FileDialog();

		bool save(const std::string& key, const std::string& title, const std::string& filter, const std::string& startingDir = "");

		bool open(const std::string& key, const std::string& title, const std::string& filter, bool isMultiselect = false, const std::string& startingDir = "");

		bool isDone(const std::string& key);

		inline bool hasResult() { return m_result.size(); }
		inline const std::filesystem::path& getResult() { return m_result[0]; }
		inline const std::vector<std::filesystem::path>& getResults() { return m_result; }

		void close();

		void removeFavorite(const std::string& path);
		void addFavorite(const std::string& path);
		inline const std::vector<std::string>& getFavorites() { return m_favorites; }

		inline void setZoom(float z) { 
			m_zoom = std::min<float>(25.0f, std::max<float>(1.0f, z)); 
			m_refreshIconPreview();
		}
		inline float getZoom() { return m_zoom; }

		std::function<void*(const uint8_t*, int, int, Format)> createTexture;
		std::function<void(void*)> deleteTexture;

	private:
		struct FileTreeNode {
#ifdef _WIN32
			FileTreeNode(const std::wstring& path) {
				this->path = std::filesystem::path(path);
				read = false;
			}
#endif

			FileTreeNode(const std::string& path) {
				this->path = std::filesystem::u8path(path);
				read = false;
			}

			std::filesystem::path path;
			bool read;
			std::vector<std::unique_ptr<FileTreeNode>> children;
		};

		struct SmartSize {
			SmartSize() = default;
			SmartSize(size_t s);

			auto operator<=>(const SmartSize& others) const noexcept
			{
				return sizeInByte <=> others.sizeInByte;
			}

			size_t sizeInByte;
			float size;
			std::string unit;
		};

		struct FileData {
			FileData(const std::filesystem::path& path);

			std::filesystem::path path;
			bool isDirectory;
			SmartSize size;
			time_t dateModified;

			bool hasIconPreview;
			void* iconPreview;
			uint8_t* iconPreviewData;
			int iconPreviewWidth, iconPreviewHeight;
		};

		std::string m_currentKey;
		std::string m_currentTitle;
		std::filesystem::path m_currentDirectory;
		bool m_isMultiselect;
		bool m_isOpen;
		uint8_t m_type;
		std::string m_inputTextbox;
		std::string m_pathBuffer;
		std::string m_newEntryBuffer;
		std::string m_searchBuffer;
		std::vector<std::string> m_favorites;
		bool m_calledOpenPopup;
		std::stack<std::filesystem::path> m_backHistory, m_forwardHistory;
		float m_zoom;

		std::vector<std::filesystem::path> m_selections;
		int m_selectedFileItem;
		void m_select(const std::filesystem::path& path, bool isCtrlDown = false);

		std::vector<std::filesystem::path> m_result;
		bool m_finalize(const std::string& filename = "");

		std::string m_filter;
		std::vector<std::vector<std::string>> m_filterExtensions;
		size_t m_filterSelection;
		void m_parseFilter(const std::string& filter);

		std::unordered_map<std::string, void*> m_icons;
		void* m_getIcon(const std::filesystem::path& path);
		void m_clearIcons();
		void m_refreshIconPreview();
		void m_clearIconPreview();

		std::thread m_previewLoader;
		bool m_previewLoaderRunning;
		void m_stopPreviewLoader();
		void m_loadPreview();

		std::vector<std::unique_ptr<FileTreeNode>> m_treeCache;
		void m_renderTree(FileTreeNode& node);

		unsigned int m_sortColumn;
		unsigned int m_sortDirection;
		std::vector<FileData> m_content;
		void m_setDirectory(const std::filesystem::path& p, bool addHistory = true);
		void m_sortContent(unsigned int column, unsigned int sortDirection);
		void m_renderContent();

		void m_renderPopups();
		void m_renderFileDialog();

		FileDialog();
	};
}
