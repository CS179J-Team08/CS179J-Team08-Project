import React, { useEffect, useState, Component } from 'react';
import { Storage, API, graphqlOperation, Auth } from 'aws-amplify'
import uuid from 'uuid/v4'
import { withAuthenticator } from 'aws-amplify-react'

// define a data packet JSON object
let packet = {
  group: [
    { userID: "" },
    { userID: "" },
    { userID: "" },
    { userID: "" },
    { userID: "" },
  ],
  filename: "",
  play: false,
  parameters: {
    volume: 0.0
  }
};

// set the JSON file prefix to the users uuid ("sub")
let jsonFilePrefix = 'default';
Auth.currentAuthenticatedUser().then((user) => {
  jsonFilePrefix = user.attributes.sub;
  packet.group[0].userID = user.attributes.sub;
})

class UploadFile extends Component {
      constructor(props) {
        super(props);

        this.state = {
          Filename: "",
          playValue: 'false'
        };

        this.updatePlayState = this.updatePlayState.bind(this);
        this.updateFileName = this.updateFileName.bind(this);
      }

      uploadAudioFile = (evt) => {
        const file = evt.target.files[0];
        const name = file.name;
    
        Storage.put(name, file, { level: 'private' }).then(() => {
          this.setState({ file: name });
        })
      }

      // send data packet to public bucket directory
      sendPacket = () => {
        packet.filename = this.state.Filename;
        packet.play = this.state.playValue;
        
        console.log(packet.group[0].userID);
        console.log(packet.filename);
        console.log(packet.play);

        Storage.put(jsonFilePrefix + '.json', packet)
        .then (result => console.log(result))
        .catch(err => console.log(err));
      }

      updatePlayState(event) {
        this.setState({ playValue: event.target.value });
      }

      updateFileName(event) {
        this.setState({ Filename: event.target.value })
      }

      render() {
        return (
            <>
              <div>
                <p>Upload a file</p>
                <input type="file" onChange={this.uploadAudioFile} />
              </div>
              <div>
                <form>
                  <label>
                    Filename:
                    <input type="text" onChange={this.updateFileName} />
                  </label>
                  <label>
                    Play/Stop
                    <select value={this.state.playValue} onChange={this.updatePlayState}>
                      <option value="true">play</option>
                      <option value="false">stop</option>
                    </select>
                  </label>
                </form>
                <button onClick={this.sendPacket}>Update Settings</button>
              </div>
            </>
        );
      }
}

export default UploadFile;