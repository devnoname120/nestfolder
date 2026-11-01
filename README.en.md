# nestfolder

Folders inside folders on the PS Vita home screen (LiveArea). A taiHEN plugin
plus an installer app.

> **Firmware 3.65 retail only.** On anything else the plugin detects a different
> SceShell and does nothing.
>
> This project was written with heavy help from an AI assistant (Claude by
> Anthropic). Every build was tested on a real PS Vita. See "About" below.

Türkçe: [README.md](README.md)

## What it does

- Open a folder that is inside another folder. Two levels: an outer folder and
  folders inside it. **Circle** or tapping outside goes back to the outer folder.
- Drop a folder onto another folder in edit mode on the home screen.
- Inside a folder, drop an app onto an inner folder to move it there.
- Rearrange, remove apps and rename inside an inner folder.
- Inner folder icons update without a restart, and the outer folder's icon
  shows the inner folder's icon too.
- New folders get a unique titleId (`FLDR00001`, ...) the moment they are created,
  so they can be nested right away.

## Install

1. In HENkaku settings, turn on **Enable Unsafe Homebrew** (the installer writes to `ur0:`).
2. Install `nestfolder_kurulum.vpk` with VitaShell and open **nestfolder Kurulum**
   (the app follows your system language).
3. On first start it backs up `ur0:shell/db/app.db` and `config.txt` once to
   `ux0:data/nestfolder/first_backup/`. This backup is never overwritten.
4. Choose **Install / update**. A confirmation screen explains exactly what will
   change. After a 5 second countdown (cancel with the back button) the changes are
   written and the Vita restarts.

Menu: **Install / update**, **Give titleIds to old folders**, **Uninstall**
(moves nested folders back to the home screen, clears the plugin's titleIds,
removes the plugin), **Restore first backup**, **Exit**.

Manual install: copy `nestfolder.suprx` to `ur0:tai/`, add
`ur0:tai/nestfolder.suprx` under `*main` in `ur0:tai/config.txt`, restart. Back up
`ur0:shell/db/app.db` yourself first.

## Known limits

- Moving an app out of an inner folder sends it to the home screen, not to the
  outer folder (no crash). Drag it into the outer folder again.
- If you drop an app onto an inner folder while the outer folder is open, the
  inner folder's new icon appears the next time you open the outer folder
  (the Shell saves folder contents when a folder closes).
- Folders created before the plugin may have no titleId; they cannot be dropped
  into another folder until you use **Give titleIds to old folders**.

## Recovery

- Problems at boot: hold **L** while starting the Vita to skip plugins, then remove
  the line from `config.txt` or run the installer's **Uninstall**.
- Broken home screen: use **Restore first backup**, or copy
  `ux0:data/nestfolder/first_backup/app.db` to `ur0:shell/db/app.db` with VitaShell
  and restart.
- **Uninstall before removing the plugin by hand.** Without the plugin, touching a
  nested folder freezes the home screen.

Settings (empty files in `ux0:data/nestfolder/`, applied after a restart):
`disable` (plugin off), `nosubicon` (no inner folder icon in the outer folder's
icon), `delay20` (old fixed 20 s start delay).

Log: `ux0:data/nestfolder/log.txt` (previous boot: `log.prev.txt`). Please attach
both when reporting a problem.

## Build

Needs [VitaSDK](https://vitasdk.org).

```
cmake -S . -B build
cmake --build build
```

Output: `build/nestfolder.suprx` and `build/nestfolder_kurulum.vpk` (contains the plugin).
GitHub Actions builds every push; tags `v*` publish a release.

PC tests for the installer logic: `bash installer/test/ci_test.sh`
(needs gcc, libsqlite3-dev, python3).

## About

I directed this project and tested every build on my own Vita; the code and the
reverse engineering of SceShell were done together with Claude (Anthropic's AI
assistant), working from logs and test results from my console. The technical
notes in `README.md` (Turkish) and the comments in `src/main.c` explain how the
hooks work.

## License

See `LICENSE` and `THIRD_PARTY_NOTICES.md`.

Not affiliated with Sony. PlayStation, PS Vita and LiveArea are trademarks of Sony
Interactive Entertainment. Use at your own risk; the plugin changes how the home
screen works and the installer edits `app.db`.
