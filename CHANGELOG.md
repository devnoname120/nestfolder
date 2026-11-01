# Changelog

## 1.0.2

- Installer: the app is now called "nestfolder" on the home screen and in its
  LiveArea; LiveArea images in English; the icon's folder no longer touches the
  edge of the round bubble.
- Plugin: no changes besides the version number.

## 1.0.1

- Nested folder icons now refresh without a restart. When you change the contents
  of an inner folder and go back, its icon is updated right away.
- The outer folder's icon now shows a small version of the inner folder's icon,
  like it does for apps. Can be turned off with `ux0:data/nestfolder/nosubicon`.
- Installer: Uninstall also clears the titleIds the plugin gave to folders
  (`FLDR` + 5 digits), after moving nested folders back to the home screen.

## 1.0.0

- First release with version numbers.
- Installer app (VPK): install / update, give titleIds to old folders,
  uninstall (moves nested folders to the home screen first), restore first backup.
  Detailed confirmation screen before every action, 5 s cancellable countdown,
  automatic restart.
- Installer follows the system language (Turkish, otherwise English) and the
  system's confirm button setting.
- One-time backup of `app.db` and `config.txt` in `ux0:data/nestfolder/first_backup/`.
- The plugin no longer makes its own `app.db` backups.
- Log lines in English.

## Before 1.0.0

Internal test builds, not released.
