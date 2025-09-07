# 🤝 Guia de Contribuição

Obrigado por querer contribuir com o **CCG – Console Card Game**!  
Aqui estão algumas orientações rápidas para mantermos o projeto organizado.

---

## 🚀 Fluxo de contribuição
1. Crie um fork do repositório
2. Crie um branch descritivo:
   ```bash
   git checkout -b feature/minha-feature
   ```
3. Implemente sua alteração
4. Confirme que o projeto compila com `make`
5. Abra um Pull Request relacionando a issue (se houver)

---

## 📏 Padrões de código
- Linguagem: **C99**
- Indentação: **2 espaços**
- Variáveis declaradas no **início do escopo**
- Notação húngara:
  - `iVariavel` → int  
  - `dVariavel` → double  
  - `szTexto` → string estática  
  - `pszTexto` → ponteiro para string  
  - `stEstrutura` → struct  
  - `pstEstrutura` → ponteiro para struct
  - `a+variavel` -> array de tipo de variavel. Ex.: aszStringLst -> char *szString[]
  - `g+variavel` -> global
- Seguir boas práticas de modularização: headers em `include/`, código em `src/`

---

## ✅ Checklist para Pull Requests
- [ ] Código compila e roda (`make` sem erros)
- [ ] Alterações testadas manualmente
- [ ] Código segue padrão do projeto
- [ ] Comentários/documentação atualizados

---

## 📬 Dúvidas?
Abra uma [Discussion](https://github.com/SEU_USUARIO/CCG/discussions) ou crie uma issue.
