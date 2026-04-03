# BOSS BBS

> A fully featured Bulletin Board System written in GW-BASIC for DOS — preserved from the 1980s.

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)
[![Language](https://img.shields.io/badge/language-GW--BASIC-blue.svg)](#)
[![Platform](https://img.shields.io/badge/platform-DOS%20%7C%20DOSBox-lightgrey.svg)](#running-with-dosbox)
[![Era](https://img.shields.io/badge/era-1980s-orange.svg)](#)

BOSS BBS (Bulletin Board System) is a complete, single-line BBS written entirely in GW-BASIC by **Mark Longo** in the late 1980s. It ran on a standard DOS PC connected to a modem, letting remote callers dial in to read messages, share files, and chat — decades before the web existed. This repository preserves the original source code and data files exactly as they were.

---

## Table of Contents

- [Features](#features)
- [Screenshots](#screenshots)
- [Running with DOSBox](#running-with-dosbox)
- [Project Structure](#project-structure)
- [Historical Context](#historical-context)
- [Contributing](#contributing)
- [License](#license)

---

## Features

| Category | Details |
|---|---|
| **User Accounts** | Registration, login, password verification, security levels |
| **Message Areas** | 15 boards (Town Centre, Movies, Sports, Debate, Fantasy, Adult, SysOp, and more) |
| **Message Editor** | Full-screen editor — insert, delete, replace, and format lines |
| **File Areas** | 7 libraries (Games, Utilities, Communications, Programmers, Windows, Private, Stomp) |
| **File Transfers** | Upload and download via Zmodem using DSZ |
| **Door Games** | Launches external programs via DORINFO1.DEF standard |
| **SysOp Chat** | Break-in real-time chat with live callers |
| **ANSI Graphics** | Optional colour terminal art support |
| **Time Limits** | Per-session countdown with low-time warnings |
| **Modem Management** | Auto-init, baud-rate detection, carrier monitoring on COM1 |

---

## Screenshots

<img src="https://github.com/moonwho101/BOSS-BBS/blob/main/BOSS/SCREEN/boss1.jpg" width="800">
<img src="https://github.com/moonwho101/BOSS-BBS/blob/main/BOSS/SCREEN/boss2.jpg" width="800">
<img src="https://github.com/moonwho101/BOSS-BBS/blob/main/BOSS/SCREEN/boss3.jpg" width="800">
<img src="https://github.com/moonwho101/BOSS-BBS/blob/main/BOSS/SCREEN/boss4.jpg" width="800">
<img src="https://github.com/moonwho101/BOSS-BBS/blob/main/BOSS/SCREEN/boss5.jpg" width="800">
<img src="https://github.com/moonwho101/BOSS-BBS/blob/main/BOSS/SCREEN/boss6.jpg" width="800">
<img src="https://github.com/moonwho101/BOSS-BBS/blob/main/BOSS/SCREEN/boss7.jpg" width="800">
<img src="https://github.com/moonwho101/BOSS-BBS/blob/main/BOSS/SCREEN/boss8.jpg" width="800">
<img src="https://github.com/moonwho101/BOSS-BBS/blob/main/BOSS/SCREEN/boss9.jpg" width="800">
<img src="https://github.com/moonwho101/BOSS-BBS/blob/main/BOSS/SCREEN/boss10.jpg" width="800">

---

## Running with DOSBox

BOSS BBS requires a DOS environment. The easiest way to run it today is with [DOSBox](https://www.dosbox.com/) or [DOSBox-X](https://dosbox-x.com/).

1. **Install DOSBox** for your platform.
2. **Mount the BOSS directory** inside DOSBox:
   ```
   mount c d:\Github\BOSS-BBS\BOSS
   c:
   ```
3. **Start GW-BASIC** and load the main program:
   ```
   gwbasic BOSS.BAS
   ```
4. The system expects a modem on **COM1**. For local testing (no modem), the code detects when no carrier is present and can run in local mode.

> **Tip:** DOSBox-X includes better support for COM port emulation if you want to test the modem path.

---

## Project Structure

```
BOSS/
├── BOSS.BAS          # Main BBS program (GW-BASIC source)
├── USERS.BBS         # User account database
├── MESSAGE.BBS       # Message base
├── MENU/             # ANSI and ASCII menu screens
├── FILE1–FILE7/      # File library directories with descriptions
├── SCREEN/           # Screenshots
├── ADVBAS40/         # Advanced BASIC library used by BOSS
├── QWIK/             # Supporting utilities
└── *.BAS / *.BAT     # Utility programs (packing, purging, events, etc.)
```

---

## Historical Context

Bulletin Board Systems were the social networks of the pre-internet era. From the late 1970s through the mid-1990s, hobbyists would dial into BBS servers using modems to exchange messages, share software, and play door games — all over standard phone lines.

BOSS BBS was written **entirely in GW-BASIC**, the Microsoft BASIC interpreter bundled with MS-DOS. Despite the constraints of interpreted BASIC, it delivered a full-featured, multi-area BBS with ANSI art, Zmodem transfers, and real-time SysOp chat. It is a testament to what dedicated hobbyist programmers could accomplish with minimal hardware.

This source code is preserved here as a piece of **retrocomputing history**.

---

## Contributing

Contributions are welcome — bug fixes, DOSBox setup guides, ANSI art improvements, or historical documentation. Please open an issue or pull request.

---

## License

MIT © Mark Longo. See [LICENSE](LICENSE) for details.

