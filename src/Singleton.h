#include <iostream>
#include <mutex>

template <typename T, typename... Args>
class Singleton {
public:
    // Método para obter a instância única do Singleton
    static T& getInstance(Args... args) {
        static T instance(std::forward<Args>(args)...);  // A instância será criada apenas uma vez com argumentos
        return instance;
    }

    // Evitar a cópia ou movimentação do objeto
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;
    Singleton(Singleton&&) = delete;
    Singleton& operator=(Singleton&&) = delete;

protected:
    // Construtor protegido para evitar criação direta da classe
    Singleton() = default;

    // Destruidor protegido
    virtual ~Singleton() = default;
};