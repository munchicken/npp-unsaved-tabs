# 🤝 Contributing

Pull requests are welcome!

Use the Git Flow model:
```
git flow feature start <feature-name>
# …code, commit…
git push origin feature/<feature-name>
# Open a PR from feature/<feature-name> → develop
# After it’s merged:
git fetch origin
git branch -d feature/<feature-name>   # or git flow feature finish <feature-name>
```
