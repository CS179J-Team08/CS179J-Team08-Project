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
  stop: false,
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
          playValue: 'false',
          volume: 0.0
        };

        this.updatePlayState = this.updatePlayState.bind(this);
        this.updateFileName = this.updateFileName.bind(this);
        this.updateVolume = this.updateVolume.bind(this);

      }

      uploadAudioFile = async (evt) => {
        // get the file and file name
        const file = evt.target.files[0];
        const name = file.name;

        // get all files currently in database
        const audioFiles = await API.graphql(graphqlOperation(queries.listAudios));
        const items = audioFiles.data.listAudios.items;

        // check if file name already exists for current user
        var fileExists = false;
        for (const i in items) {
          if(items[i].name == name) {
            fileExists = true;
          }
        }
        
        if(!fileExists) {
          // define metadata for the file
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
            // put file into S3
            await Storage.put(name, file, { contentType: 'mimeType' }).then(() => {
              this.setState({ file: name });
            })
            .catch(err => console.log(err));

            // create pointer in dynamoDB
            await API.graphql(graphqlOperation(mutations.createAudio, {input: audioFileDetails}));

            window.alert('File successfully uploaded');
          } catch (err) {
            console.log('error: ', err);
          }
        } else {
          window.alert('Selected file already exists in your database.');
        }
      }

      // send data packet to public bucket directory
      sendPacket = () => {
        packet.filename = this.state.Filename;
        packet.play = this.state.playValue;
        packet.parameters.volume = this.state.volume;
        
        // debugging purposes
        console.log(packet.group[0].userID);
        console.log(packet.filename);
        console.log(packet.play);
        console.log(packet.volume);

        Storage.put(jsonFilePrefix + '.json', packet)
        .then (result => console.log(result))
        .catch(err => console.log(err));
      }

      updatePlayState(event) {
        this.setState({ playValue: event.target.value });
      }

      updateFileName(event) {
        this.setState({ Filename: event.target.value });
      }

      updateVolume(event) {
        this.setState({ volume: event.target.value });
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
                  <label>
                    Volume:
                    <input type="number" step="0.1" onChange={this.updateVolume} />
                  </label>
                </form>
                <button onClick={this.sendPacket}>Send</button>
              </div>
            </>
        );
      }
}

export default UploadFile;