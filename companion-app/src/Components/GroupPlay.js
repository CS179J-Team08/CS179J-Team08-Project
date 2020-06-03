import React, { Component } from 'react';
import { Auth, API, Storage } from 'aws-amplify';

// it seems the best course of action is to create a new packet definition for group play 
// since we want a clean slate when sending a packet for group sharing
// researching online shows that clearing our a javascript object is best done
// by just creating a new object. I guess.
let groupPacket = {
    group: [
        { userID: "" },
        { userID: "" },
        { userID: "" },
        { userID: "" },
        { userID: "" },
    ],
    filenames: [ // dynamic
        { name: "" }
    ],
    play: "false",
    stop: "false",
    parameters: {
      volume: "0.0",
      echo: {
        apply: "false",
        delay: "10.0",
        feedback: "0.0",
        dry: "0.0",
        wet: "0.0"
      },
      equalizer: {
        apply: "false",
        lowgain: "0.0",
        midgain: "0.0",
        highgain: "0.0"
      }
    }
};

// this apparently only works in the global scope
let currentUserId = 'default';
Auth.currentAuthenticatedUser().then((user) => {
    currentUserId = user.attributes.sub;
});

export default class GroupPlay extends Component {
    constructor(props) {
        super(props);

        this.state = {
            usernames: {
                user0: "",
                user1: "",
                user2: "",
                user3: "",
                user4: ""
            },
            numUsers: 0,
            userIDs: [],
            groupFileNames: [{ name: "" }],
            play: "false",
            stop: "false"
        }

        this.addUsersToGroup = this.addUsersToGroup.bind(this);
        this.shareFile = this.shareFile.bind(this);
    }

    addUsersToGroup = async (event) => {
        event.preventDefault();

        this.setState({ userIDs: [] });

        let usersObj = this.state.usernames;
        let usersToAdd = Object.keys(usersObj).map(function(key) {
            return usersObj[key];
        });

        let apiName = "AdminQueries";
        let path = "/getUser"

        let userCount = 0;
        let userAdded = false;
        for (const i in usersToAdd) {
            if(usersToAdd[i] !== "") {
                let myInit = {
                    queryStringParameters: {
                        "username" : usersToAdd[i]
                    },
                    headers: {
                        'Content-Type' : 'application/json',
                        Authorization: `${(await Auth.currentSession()).getAccessToken().getJwtToken()}`
                    }
                }

                let userDetails = await API.get(apiName, path, myInit);
                let userIDTemp = this.state.userIDs.concat();
                userIDTemp.push(userDetails['UserAttributes'][0]["Value"])
                this.setState({
                    userIDs: userIDTemp
                });

                userAdded = true;
                ++userCount;

                this.setState({
                    numUsers: userCount
                });
            } 
        }

        if(!userAdded) {
            window.alert("Please specify at least one user");
        } else {
            let userIDTemp = this.state.userIDs.concat();
            userIDTemp.push(currentUserId);

            this.setState({
                userIDs: userIDTemp
            })
        }

        console.log(this.state.userIDs);
    }

    shareFile = () => {
        let groupUsers = this.state.userIDs;
        for (const i in groupUsers) {
            groupPacket.group[i].userID = groupUsers[i];
        }

        groupPacket.filenames = this.state.groupFileNames;
        
        // for debugging
        console.log(groupPacket);

        // assign json file prefixes as per each user's UUID
        let prefix = 'default';
        for (const i in groupUsers) {
            prefix = groupUsers[i];
        }
        Storage.put(prefix + '.json', groupPacket)
        .then(result => console.log(result))
        .catch(err => console.log(err));
    }

    togglePlayPause = () => {
        let groupUsers = this.state.userIDs;

        let play = this.state.play;
        if(play === "true") {
            play = "false";
        } else {
            play = "true";
        }

        this.setState({ play: play });
        this.setState({ stop: "false" });
        groupPacket.play = this.state.play;
        groupPacket.stop = this.state.stop;

        let prefix = 'default';
        for (const i in groupUsers) {
            prefix = groupUsers[i];
        }
        
        console.log(this.state);

        Storage.put(prefix + '.json', groupPacket)
        .then(result => console.log(result))
        .catch(err => console.log(err));
    }

    toggleStop = () => {
        let stop = this.state.stop;
        if(stop === "false") {
            stop = "true";
        } else {
            stop = "false";
        }

        this.setState({ stop: stop })
        groupPacket.stop = this.state.stop;

        let prefix = 'default';
        let groupUsers = this.state.userIDs;
        for (const i in groupUsers) {
            prefix = groupUsers[i];

        }
    
        Storage.put(prefix + '.json', groupPacket)
        .then(result => console.log(result))
        .catch(err => console.log(err));
    }

    updateFileName = idx => event => {
        const newFilenames = this.state.groupFileNames.map((filename, sidx) => {
          if(idx !== sidx) {
            return filename;
          }

          return { ...filename, name: event.target.value };
        });

        this.setState({ groupFileNames: newFilenames })
      }

    removeFilename = idx => () => {
        this.setState({
            groupFileNames: this.state.groupFileNames.filter((s, sidx) => idx !== sidx)
        });
    }

    addFilename = () => {
        this.setState({
            groupFileNames: this.state.groupFileNames.concat([{ name: "" }])
        });
    }

    render() {
      return (
        <>
            <div>
                <h3>Add Users to Group</h3>
                <form onSubmit={ this.addUsersToGroup }>
                    <label>
                        First User
                        <input type="text" onChange={ (event) => {
                            let newUsernames = this.state.usernames;
                            newUsernames.user0 = event.target.value;
                            this.setState({
                                usernames: newUsernames
                            });
                        } } />
                    </label>
                    <label>
                        Second User
                        <input type="text" onChange={ (event) => {
                            let newUsernames = this.state.usernames;
                            newUsernames.user1 = event.target.value;
                            this.setState({
                                usernames: newUsernames
                            });
                        } } />
                    </label>
                    <label>
                        Third User
                        <input type="text" onChange={ (event) => {
                            let newUsernames = this.state.usernames;
                            newUsernames.user2 = event.target.value;
                            this.setState({
                                usernames: newUsernames
                            });
                        } } />
                    </label>
                    <label>
                        Fourth User
                        <input type="text" onChange={ (event) => {
                            let newUsernames = this.state.usernames;
                            newUsernames.user3 = event.target.value;
                            this.setState({
                                usernames: newUsernames
                            });
                        } } />
                    </label>
                    <label>
                        Fifth User
                        <input type="text" onChange={ (event) => {
                            let newUsernames = this.state.usernames;
                            newUsernames.user4 = event.target.value;
                            this.setState({
                                usernames: newUsernames
                            });
                        } } />
                    </label>
                    <input type="submit" value="Add Users" />
                </form>
            </div>
            <div>
                <form>
                  <h3> Share Audio </h3>
                    { this.state.groupFileNames.map((filename, idx) => (
                      <div>
                        <input 
                          type="text"
                          placeholder={`File #${idx + 1}`}
                          value={filename.name}
                          onChange={this.updateFileName(idx)}
                        />
                        <button
                          type="button"
                          onClick={this.removeFilename(idx)}
                        >
                          Remove
                        </button>
                      </div>
                    )) }
                    <button
                      type="button"
                      onClick={this.addFilename}
                    >Add File</button>
                </form>
                <p></p>
                <button onClick={this.shareFile}>Share Playlist</button>
            </div>
            <div>
                <p></p>
                <button onClick={this.togglePlayPause}>Play/Pause</button>
                <button onClick={this.toggleStop}>Stop</button>
            </div>
        </>
      );
    }
  }