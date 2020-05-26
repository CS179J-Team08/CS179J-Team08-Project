# CloudSound
CloudSound aims to bring the experience of enjoying music among friend or individuals with similar interests in music. Through the use of AWS services, our web application allows users to connect their CloudSound device to their account. Where they can upload/download files, connect and make friends, manage a their file lists, and participate in a group music sessions.

## Getting Started
These instructions will get you a copy of the project up and running on your local machine.

### Prerequisites
The following list breaks up what needs to be downloaded before you can get the project up and running.

#### Companion App
* [AWS CLI for Windows](https://docs.aws.amazon.com/cli/latest/userguide/install-cliv2-windows.html) - Guide for Windows Installation
* [AWS CLI for Linux](https://docs.aws.amazon.com/cli/latest/userguide/install-cliv2-linux.html) - Guide for linux installation
```
curl "https://awscli.amazonaws.com/awscli-exe-linux-x86_64.zip" -o "awscliv2.zip"
unzip awscliv2.zip
sudo ./aws/install
```
* [NPM](https://nodejs.org/en/download/) - Package manager that allows for install of Amplify CLI

* [Amplify CLI](https://docs.amplify.aws/cli/start/install) - Framework that integrates AWS services into our webapp
```
npm install -g @aws-amplify/cli
```
* Configure an enviroment - Once the amplify CLI is installed, you will need to configure a IAM user and enviroment. It will prompt you for input and the region we use is us-west-2.
```
amplify configure
```

#### CloudSound device

* Python version 3.7, but should would with 3.5 and higher.

* Pip3 - Package manager for Python
```
sudo apt-get install pip3
```
* Boto3 - AWS Python SDK
```
pip3 install boto3
```
* AWS CLI v.1 for Linux - install using pip3
```
pip3 install awscli --upgrade --user
```

* Pytest - testing tools for Python
```
pip3 install pytest
```
### Installing
These steps will guide to setting up both the companion app and the cloudsound device.

#### Companion-app
When you clone the repo, cd into the companion-app directory and run the following command.
```
sudo npm install
```
That should install any necessary packages need for the project, now we need to grab any dependencies for the AWS services.It will prompt you to choose whether or not you wish to use an already existing enviroment. This should have been done previously with the amplify configure command in prerequisites step.
```
amplify init
```
Lastly to run the companion-app you need to use the following command.
```
npm start
```
With this all done you should have started the development server and the web should appear in a browser tab. If the app does not appear open a broswer and type into the url
```
https://localhost:3000
```

#### CloudSound Device
All the needs to be done on the device is run the two executables that allow cloudsound to recieve packets and initiate audio playback

* pyServer - Packet reception located in the pyServer/ directory of the project.
```
python3 server.py
```

* CloudSound - Audio playback located in AudioEngine/ directory of the project. First cd into the directory and make the executable on the device.
```
make build CPU=armhf CONFIG=Release
```
Then once the cloudsound executable is built
```
./cloudsound
```

It should be noted that we need to run the server.py first before the cloundsound executable is ran.

### Running tests
The tests for each component can be ran using the following comands.

* CloudSound - Ensure you are in the AudioEngine/ directory
```
make test CPU=armhf CONFIG=Release
./audio_test
```

* pyServer - Ensure you are in the pyServer/ directory
```
pytest
```

* Companion-app - Ensure you are in the Companion-app/ directory
```
npm test
```
