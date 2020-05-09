import React, { useEffect, useState, Component } from 'react';
import { Storage, API, graphqlOperation, Auth } from 'aws-amplify'
import uuid from 'uuid/v4'
import { withAuthenticator } from 'aws-amplify-react'
import * as mutations from '../graphql/mutations';
import * as queries from '../graphql/queries';

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

      uploadAudioFile = async (evt) => {
        const file = evt.target.files[0];
        const name = file.name;
        const audioFile = {
          bucket: 'cs-audiofile-bucketdefault-default',
          region: 'us-west-2',
          key: '/public/' + name
        };
        
        const audioFileDetails = {
          name: name,
          file: audioFile
        };

        try {
          await Storage.put(name, file, { contentType: 'mimeType' }).then(() => {
            this.setState({ file: name });
          })
          .catch(err => console.log(err));

          await API.graphql(graphqlOperation(mutations.createAudio, {input: audioFileDetails}));
          console.log('File successfully added');
        } catch (err) {
          console.log('error: ', err);
        }
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
                <h3>Upload a file</h3>
                <input type="file" onChange={this.uploadAudioFile} />
              </div>
              <div>
                <h3>Send file to device</h3>
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
                <button onClick={this.sendPacket}>Send</button>
              </div>
            </>
        );
      }
}

export default UploadFile;