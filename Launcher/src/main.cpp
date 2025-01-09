#include "app-window.h"
#include "settings.h"
#include <string>
#include <shobjidl.h> 

// Função auxiliar para converter std::string para std::wstring
std::wstring string_to_wide(const std::string& str) {
    if (str.empty()) return std::wstring();
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
    std::wstring wstrTo(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
    return wstrTo;
}

// Função auxiliar para converter std::wstring para std::string
std::string wide_to_string(const std::wstring& wstr) {
    if (wstr.empty()) return std::string();
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
    std::string strTo(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
    return strTo;
}

std::string browse_folder() {
    std::string result;
    
    // Inicializa COM
    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    if (FAILED(hr)) return result;

    IFileDialog *pfd;
    hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, 
                         IID_IFileOpenDialog, reinterpret_cast<void**>(&pfd));
    
    if (SUCCEEDED(hr)) {
        DWORD dwOptions;
        hr = pfd->GetOptions(&dwOptions);
        if (SUCCEEDED(hr)) {
            pfd->SetOptions(dwOptions | FOS_PICKFOLDERS);
        }
        
        hr = pfd->Show(NULL);
        
        if (SUCCEEDED(hr)) {
            IShellItem *psi;
            hr = pfd->GetResult(&psi);
            if (SUCCEEDED(hr)) {
                PWSTR path;
                hr = psi->GetDisplayName(SIGDN_FILESYSPATH, &path);
                if (SUCCEEDED(hr)) {
                    result = wide_to_string(path);
                    CoTaskMemFree(path);
                }
                psi->Release();
            }
        }
        pfd->Release();
    }
    
    CoUninitialize();
    return result;
}

#ifdef __linux__
int main(int argc, char **argv) 
#else
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
#endif
{
    auto window = EngineSelector::create();

    window->on_browse([window]()
    {
        std::string selected_path = browse_folder();
        if (!selected_path.empty()) {
            window->set_project_path(slint::SharedString(selected_path));
        }
    });

    window->on_open_settings([window]() {
        Windows::open_settings();
    });
    
    // Manipulador para o botão "Iniciar Engine"
    window->on_start_engine([window]() {
        std::string project_path = window->get_project_path().data();
        if (project_path.empty()) {
            MessageBoxA(NULL, "Por favor, selecione um caminho para o projeto.", "Aviso", MB_ICONWARNING);
            return;
        }
        
        // Aqui você pode adicionar a lógica para iniciar o engine
        // MessageBoxA(NULL, "Iniciando Engine...", "Informação", MB_ICONINFORMATION);
    });

    window->run();
    return 0;
}
