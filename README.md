
## Password Helper Device

Use different strong passwords for every service and:

- you don't need to remember them
- you don't need a software to be installed on the machine where you want to authenticate, so you can use it everywhere (home/work...)
- you can use it for BIOS password
- you can use it with any fully encrypted computer startup mechanism that requires a password
- you don't need the service provider to make any change to its password authentication mechanism (works with facebook, google...)
- it's easier to manage than a paper under your keyboard, it's also safer
- it's OpenSource


##### Anywhere you type a password on your keyboard, this solution fit.

### Project :
A "Token" device that generates secure passwords and encrypt them using a 128 bits AES key. The secret key is stored in the token's eeprom memory only.

The generated encrypted password is sent to a Master Device (let's say : an Iphone) to be stored (the token device does not store the generated password, nor the encrypted version).

- The token knows a secret AES128 symmetric key
- The Iphone knows a bunch of ciphered passwords that cannot be decrypted without the token

### How to use this?
When you want to login somewhere, instead of typing a password:

- Plug the Token to the computer (USB)
- Run the Iphone App (work in progress) and select the account's service you try to log-in (ex: workstation/gmail/facebook/amazon...)
- The Iphone App sends the associated encrypted password to the Token, that can decrypt it (because it knows the secret key) and the token "types" the password where your focus is.


### How does it work?
The token looks like a USB keyboard to your computer. It can send keystrokes like a human will do on its own keyboard.


###What about security?
The security is "almost" the same when you type your password by yourself. Almost, because:
- You don't have to remember the passwords, so they can be unique for every service, strong, and changed easily.


### What about security (details)?

- Password are encrypted using an AES128 key (CBC operation mode)
- Entropy : the token uses the [Entropy Library](https://sites.google.com/site/astudyofentropy/project-definition/timer-jitter-entropy-sources/entropy-library)
- The communication protocol (over BLE) uses a HMAC-SHA256 based challenge (64bit random nonce)/response to authenticate the sender and to ensure that there is no replay in the air. So yes, there is another 128bit AES key. This one is shared during pairing, and saved inside the Iphone App
- The lockbits are useful to reduce the risk of a physical access to the Token (they are not set...yet)
- During pairing, the token keys are sent to the Iphone in clear-text (to understand why, report to the "What if I lose my Token"). This could be better, but it appends only once. You can do it inside a [faraday cage](http://www.instructables.com/id/Faraday-Cage-Phone-Pouch/) if you are the absolute paranoid type of person ;).
- The phone does not store the secret key used to decrypt password.
- The phone has 2 keys : the one used to authenticate the challenge/response, and another one, for later purpose: one on the next objective is to be able to use the token in a pocket, powered by a battery (that will be charged we the Token is pluged-in), to access passwords anywhere, by printing them temporarily on the phone itself - or clipboard -).
- So yes, there are 3 keys : 
  - 1 to cipher the password, stored inside the Token EEPROM
  - 1 to authenticate the sender of a command using a HMAC-SHA256 based challenge/response, stored either on the phone and the Token
  - 1 to securely send back to the phone a password decrypted by the token, to print it on screen temporarily, or to put it inside the clipboard for a paste operation (ex : you want to authenticate when browsing with your phone). This one is stored on the phone and the Token.

### What if I lose my token?
- It's OK, you can create a new one, and recover your passwords:

When you pair the token with your phone for the first time, the secrets keys of the Token are sent to the phone.
The phone encrypt them with a generated AES key, (let's call it the Recovery Key, it's generated for simplicity, but you can override it using your own strong passphrase that will be derivated (PBKDF2) to generate the Recovery Key).
The application will ask you to print this secret Recovery Key (as a QR code), and put it in a safe somewhere...safe.

So yes, there are 4 keys (and that's the last, I promise).

The Recovery Key rules them all, keep it safe.

### What if I lose my phone?
- It's OK, your encrypted passwords can be safely stored on any Cloud, synchronized across your devices, or simply backuped by mail.
They are encrypted, and only the token has the key to decrypt them.
 

### What do I need?
Used material :

- The Token : Bluetooth 4.0 (Bluetooth Low Energy or BLE) enabled device, powered by an atmega32u4 (arduino), like this one : [Blend Micro](http://redbearlab.com/blendmicro)


![Blend Micro](http://static1.squarespace.com/static/5039e08be4b00cf0e8cf88cd/t/5369db8fe4b0968802ea163d/1399446417765/BlendMicro.F%26B.jpg?format=200w)

- A bluetooth 4.0 compatible phone (like iPhone >= 4s)

### What about this solution vs other closed-source solution?
I don't know anything about any other solution (in general), and I don't care.
I designed and made this one because I won't trust anyone else solution to manage my passwords.

This project is also a way to play with arduino stuff and geek during my free time.
