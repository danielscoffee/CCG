# 🃏 CCG – Console Card Game

Um jogo de cartas colecionáveis **em C (ANSI/C89/C99)**, rodando direto no **terminal**.  
O projeto é pensado para ser **leve, didático e open source**, servindo tanto como jogo quanto como exemplo de arquitetura em C.

---

## ✨ Features principais

- Sistema de **jogadores** e **monstros**
- **Decks** configuráveis
- Mercado / loja para compra de cartas
- Módulo de **batalha** entre jogadores
- Interface via **terminal**
- Estrutura modular (`src/` + `include/`)
- Makefile estruturado
- Portabilidade Win32 e Linux
- Rotina de debug simples e robusta (modulo trace -> log/card_game.log)

---

## 📂 Estrutura do projeto

```
├── include/          # Cabeçalhos (.h)
├── src/              # Implementações (.c)
├── scripts/          # scripts de compilação para prod/debug/fake (.sh)
├── doc/              # documentacoes (.md)
├── Makefile          # Build simplificado
├── .gitignore
└── README.md
```
## 📂 Diretorios temporários

```
├── obj/              # Arquivos objetos da compilacao (.o)
├── log/              # Registros da execucao (.log)
├── bin/              # Local onde o executavel sera gerado
├── Makefile          # Build simplificado
├── .gitignore
└── README.md
```


---

## 🛠️ Como compilar

Requisitos: GCC ou Clang + Make

```bash
# Clonar o projeto
git clone https://github.com/SEU_USUARIO/CCG.git
cd CCG

# Compilar
Windows:
./scripts/mk*_win32.sh
Linux:
./scripts/mk*_linux.sh
Onde:
 * => all - Executa o Clean + criacao de diretorios ( usualmente o primeiro make do dia deve ser este )
 * => d - Debug flags de gdb
 * => (apenas mk_win32 ou mk_linux) - monta apenas os arquivos alterados desde a ultima compilacao
# Executar
./ccg
```

Limpar artefatos:

```bash
make clean
```

---

## 🤝 Como contribuir

1. Faça um **fork** do repositório
2. Crie um branch:

   ```bash
   git checkout -b feature/minha-feature
   ```

3. Commit suas mudanças seguindo o padrão [Conventional Commits](https://www.conventionalcommits.org/):

   ```bash
   git commit -m "feat: adicionar nova funcionalidade"
   git commit -m "fix: corrigir bug no sistema de batalha"
   ```

4. Abra um **Pull Request** 🎉

**Configure os Git hooks** para validação automática:

```bash
./scripts/setup-git-hooks.sh
```

Consulte o arquivo [CONTRIBUTING.md](CONTRIBUTING.md) para mais detalhes sobre padrões de commit e código.

---

## 🗺️ Roadmap inicial

- [ ] Melhorar loop de batalha
- [ ] Adicionar testes unitários
- [ ] Suporte a diferentes modos de jogo
- [ ] Localização (PT/EN)

---
