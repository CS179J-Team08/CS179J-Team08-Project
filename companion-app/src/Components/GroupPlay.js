import React, { Component } from 'react';
import { Auth, API, Storage } from 'aws-amplify';

// it seems the best course of action is to create a new packet for group play 
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
    filename: "",
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

export default class App extends Component {
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
            groupFileName: "",
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
                })
            } 
        }

        console.log(this.state.userIDs);

        if(!userAdded) {
            window.alert("Please specify at least one user");
        }
    }

    shareFile = async () => {
        let groupUsers = this.state.userIDs;
        for (const i in groupUsers) {
            groupPacket.group[i].userID = groupUsers[i];
        }

        groupPacket.filename = this.state.groupFileName;
        
        // for debugging
        console.log(groupPacket);

        // here for reference, if this is still exists please remove it
/* 
        Storage.put(jsonFilePrefix + '.json', packet)
        .then (result => console.log(result))
        .catch(err => console.log(err));
*/
        // assign json file prefixes as per each user's UUID
        for (const i in groupUsers) {
            let prefix = groupUsers[i];

            Storage.put(prefix + '.json', groupPacket)
            .then(result => console.log(result))
            .catch(err => console.log(err));
        }
    }

    togglePlayPause = () => {
        let groupUsers = this.state.userIDs;

        let play = this.state.play;
        if(play === "false") {
            play = "true";
        } else {
            play = "false";
        }

        this.setState({ play: play });
        groupPacket.play = this.state.play;

        for (const i in groupUsers) {
            let prefix = groupUsers[i];

            Storage.put(prefix + '.json', groupPacket)
            .then(result => console.log(result))
            .catch(err => console.log(err));
        }

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

        let groupUsers = this.state.userIDs;
        for (const i in groupUsers) {
            let prefix = groupUsers[i];

            Storage.put(prefix + '.json', groupPacket)
            .then(result => console.log(result))
            .catch(err => console.log(err));
        }
    }

    render() {
      return (
        <>
            <div>
                <p>TODO: Add group play functionality</p>
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
                <h3>Share Audio</h3>
                <label>
                    Filename:
                    <input type="text" onChange={ (event) => {
                        this.setState({
                            groupFileName: event.target.value
                        })
                    } } />
                </label>
                <button onClick={this.shareFile}>Share</button>
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