
## Password Helper Device

Use different strong passwords for every service and:

- you don't need to remember them
- you don't need a software to be installed on the machine where you normally type your password, so you can use it everywhere (home/work...)
- you can use it for BIOS password
- you can use it with any fully encrypted computer startup mechanism that requires a password
- you don't need the service provider to make any change to its password authentication mechanism (works with facebook, google...)
- it's easier to manage than a paper under your keyboard, it's also safer
- it's OpenSource


##### Anywhere you type a password on your keyboard, this solution fit.

### Project :
A "Token" device that generates secure passwords and encrypt them using a 128 bits AES key. The secret key is stored in the token's eeprom memory only.

The generated encrypted password is sent to a Master Device (let's say : an iPhone) to be stored (the token does not store the generated password, nor the encrypted version).

- The token knows a secret AES128 symmetric key to encrypt/decrypt passwords.
- The iPhone knows a bunch of ciphered passwords that cannot be decrypted without the token.

### How to use this?
When you want to login somewhere, instead of typing a password:

- Plug the Token to the computer (USB)
- Run the iPhone App (work in progress) and select the account's service you try to log-in (ex: workstation/gmail/facebook/amazon...)
- The iPhone App sends the associated encrypted password to the Token, that can decrypt it (because it knows the secret key) and the Token "types" the password where your focus is.


### How does it work?
The Token looks like a USB keyboard to your computer. It can send keystrokes like a human will do on its own keyboard.


###What about security?
The security is "almost" the same when you type your password by yourself. Almost, because:
- You don't have to remember the passwords, so they can be unique for every service, strong, and changed easily.


### What about security (details)?

- Passwords are encrypted using an AES128 key (CBC operation mode), stored inside token eeprom memory.
- Entropy : the token uses the [Entropy Library](https://sites.google.com/site/astudyofentropy/project-definition/timer-jitter-entropy-sources/entropy-library)
- The communication protocol (over BLE) uses a HMAC-SHA256 based challenge (64bit random nonce)/response to authenticate the sender and to ensure that there is no replay in the air. So yes, there is another secret key, and it is also 128 bit long. This one is shared during pairing, and saved inside the iPhone App
- The lockbits will be useful to reduce the risk of a physical access to the Token (ex : external read/write token flash/eeprom memory). They are not set, yet.
- During pairing, the token keys are sent to the iPhone in clear-text (to understand why, report to the _**What if I lose my Token**_ section). This could be better, but it happens only once. You can do it inside a [faraday cage](http://www.instructables.com/id/Faraday-Cage-Phone-Pouch/) if you are the absolute paranoid type of person ;).
- The phone does not store the secret key used to decrypt password.
- The phone has 2 keys : the one used to authenticate the challenge/response, and another one, for later purpose: the next objective is to be able to use the token in a pocket, powered by a battery (that will be charged when the Token is pluged-in), to access passwords anywhere, by printing them temporarily on the phone itself - or clipboard.
- So yes, there are 3 keys : 
  - 1 to cipher the password, stored inside the Token EEPROM
  - 1 to authenticate the sender of a command using a HMAC-SHA256 based challenge/response, stored in the phone and in the Token
  - 1 to securely send back to the phone a password decrypted by the token, to print it on screen temporarily, or to put it inside the clipboard for a paste operation (ex : you want to authenticate when browsing with your phone). This one is stored in the phone and in the Token.

### What if I lose my token?
- It's OK, you can create a new one, and recover your passwords:

When you pair the token with your phone for the first time, the secrets keys of the Token are sent to the phone.
The phone encrypt them with a generated AES key. Let's call it the Recovery Key. It's generated for simplicity, but you can override it using your own strong passphrase that will be derivated (PBKDF2) to generate the Recovery Key.
The application will ask you to print this secret Recovery Key (as a QR code), and put it in a safe somewhere...safe.

So yes, there are 4 keys (and that's the last, I promise).

- The token security rely on electronic. Lock-bits will be activated to protect it's memory against external read/write. It would also be fun to imagine a secure enclosure, that breaks if someone tries to open it, so that you know that someone attempted something.

The Recovery Key rules them all, so keep it safe.

### What if I lose my phone?
- It's OK, your encrypted passwords can be safely stored on any Cloud, synchronized across your devices, or simply backuped by mail.
They are encrypted, and only the token has the key to decrypt them.
 
### What if I lose both my phone and the token
- That's the worst case scenario.

If this happens, then the security rely mostly on the strength of the mechanism you use to unlock your phone.
If you think this scenario may occurs, then you should use a phone identification mechanism that is hard to reproduce if someone is looking, or to guess if someone know you.
Ex : use digital fingerprint identification when available.

Anyway, thanks to your Recovery Key (see above), you will be able to use your passwords' backup (encrypted) to retreive them all (clear).

### What do I need?
Used material :

- The Token : Bluetooth 4.0 (Bluetooth Low Energy or BLE) enabled device, powered by an atmega32u4 (arduino), like this one : [Blend Micro](http://redbearlab.com/blendmicro)


![Blend Micro](http://static1.squarespace.com/static/5039e08be4b00cf0e8cf88cd/t/5369db8fe4b0968802ea163d/1399446417765/BlendMicro.F%26B.jpg?format=200w)

- A bluetooth 4.0 compatible phone (like iPhone >= 4s)

### Where is the iPhone App?
Work in progress, but I'm a total newbie in GUI design, so I'm open to any contribution.

The iPhone app will also be OpenSource, and I will find a way to prove that the binary signed version on the Appstore matches the sources.

### What about this solution vs other closed-source solution?
I don't know much about any other solution (in general), and I may reinvent the wheel.
Solutions I know about always require to install a software on the machine where you want to "autologin". This type of solution doesn't fit my personal requirements (ex: bios password or fully encrypted computer startup). Also, this was fun to design and make:
- to me, this password stuff is an everyday problem
- the cryptographic challenge behind this problem is cool
- it's very cool to make something blink and interact with "physical" world (I'm not used to microcontroller programming, in fact, this is my second Arduino project)
- that would be my first iPhone App
- Bluetooth 4.0 looks cool
- Apple's Swift looks cool
- the blend micro looks cool
- I would love to share it and find someone who think this is cool, too

--------------------------------
### Contributing

Contributions or suggestions are welcome. Do not hesitate to fill issues, send pull requests, or discuss by email at amoriello.hutti@gmail.com.
