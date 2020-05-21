import React, { Component } from 'react';
import { Storage, API, graphqlOperation, Auth } from 'aws-amplify'
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
    volume: 0.0,
    echo: {
      apply: false,
      delay: 10.0,
      feedback: 0.0,
      dry: 0.0,
      wet: 0.0
    },
    equalizer: {
      apply: false,
      lowgain: 0.0,
      midgain: 0.0,
      highgain: 0.0
    }
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
          playValue: "true",
          stop: "false",
          volume: "0.0",
          eqapply: "false",
          eqlowgain: "0.0",
          eqmidgain: "0.0",
          eqhighgain: "0.0",
          echoapply: "false",
          echodelay: "10.0",
          echofeedback: "0.0",
          echodry: "0.0",
          echowet: "0.0"
        };

        this.updatePlayState = this.updatePlayState.bind(this);
        this.updateFileName = this.updateFileName.bind(this);
        this.updateVolume = this.updateVolume.bind(this);
        this.applyEq = this.applyEq.bind(this);
        this.updateHighEq = this.updateHighEq.bind(this);
        this.updateMidEq = this.updateMidEq.bind(this);
        this.updateLowEq = this.updateLowEq.bind(this);
        this.applyEcho = this.applyEcho.bind(this);
        this.updateEchoDelay = this.updateEchoDelay.bind(this);
        this.updateEchoFeedback = this.updateEchoFeedback.bind(this);
        this.udpateEchoWetVolume = this.udpateEchoWetVolume.bind(this);
        this.udpateEchoDryVolume = this.udpateEchoDryVolume.bind(this);
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
          if(items[i].name === name) {
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
            const newAudio = await API.graphql(graphqlOperation(mutations.createAudio, {input: audioFileDetails}));

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
        packet.stop = this.state.stop;
        packet.parameters.volume = this.state.volume;
        packet.parameters.equalizer.apply = this.state.eqapply;
        packet.parameters.equalizer.lowgain = this.state.eqlowgain;
        packet.parameters.equalizer.midgain = this.state.eqmidgain;       
        packet.parameters.equalizer.highgain = this.state.eqhighgain;
        packet.parameters.echo.apply = this.state.echoapply;
        packet.parameters.echo.delay = this.state.echodelay;
        packet.parameters.echo.feedback = this.state.echofeedback;
        packet.parameters.echo.wet = this.state.echowet;
        packet.parameters.echo.dry = this.state.echodry;
        
        // debugging purposes
        console.log(packet);
//        console.log(packet.parameters.echo);

        Storage.put(jsonFilePrefix + '.json', packet)
        .then (result => console.log(result))
        .catch(err => console.log(err));
      }

      updatePlayState(event) {
        if(event.target.value === "stop") {
          this.setState({ stop: "true" });
        }

        if(event.target.value === "play") {
          this.setState({ playValue: "true", stop: "false" });
        }

        if(event.target.value === "pause") {
          this.setState({ playValue: "false", stop: "false" });
        }
      }

      updateFileName(event) {
        this.setState({ Filename: event.target.value });
      }

      updateVolume(event) {
        this.setState({ volume: event.target.value });
      }

      applyEq(event) {
        if(event.target.checked) {
          this.setState({ eqapply: "true" });
        } else {
          this.setState({ eqapply: "false" });
        }
      }

      updateLowEq(event) {
        this.setState({ eqlowgain: event.target.value });
      }

      updateMidEq(event) {
        this.setState({ eqmidgain: event.target.value });
      }

      updateHighEq(event) {
        this.setState({ eqhighgain: event.target.value });
      }

      applyEcho(event) {
        if(event.target.checked) {
          this.setState({ echoapply: "true" });
        } else {
          this.setState({ echoapply: "false" });
        }
      }

      updateEchoDelay(event) {
        this.setState({ echodelay: event.target.value });
      }
      
      updateEchoFeedback(event) {
        this.setState({ echofeedback: event.target.value });
      }

      udpateEchoWetVolume(event) {
        this.setState({ echowet: event.target.value });
      }

      udpateEchoDryVolume(event) {
        this.setState({ echodry: event.target.value });
      }

      render() {
        return (
            <>
              <div>
                <h2>Upload a file</h2>
                <input type="file" onChange={this.uploadAudioFile} />
              </div>
              <div>
                <h2>Play file</h2>
                <label>
                  Filename:
                  <input type="text" onChange={this.updateFileName} />
                </label>
                <label>
                  Play/Pause/Stop
                  <select onChange={this.updatePlayState}>
                    <option value="play">play</option>
                    <option value="pause">pause</option>
                    <option value="stop">stop</option>
                  </select>
                </label>
                <label>
                  Volume:
                  <input type="number" step="0.1" onChange={this.updateVolume} />
                </label>
              </div>
              <h3>Effects</h3>
              <div>
                <h5>EQ:</h5>
                <input type="checkbox" onChange={this.applyEq} />
                <label>
                  Low Gain:
                  <input type="number" step="0.1" onChange={this.updateLowEq} />
                </label>
                <label>
                  Mid Gain:
                  <input type="number" step="0.1" onChange={this.updateMidEq} />
                </label>
                <label>
                  High Gain:
                  <input type="number" step="0.1" onChange={this.updateHighEq} />
                </label>
              </div>
              <div>
                <h5>Echo:</h5>
                <input type="checkbox" onChange={this.applyEcho} />
                <label>
                  Delay:
                  <input type="number" step="0.1" onChange={this.updateEchoDelay} />
                </label>
                <label>
                  Feedback:
                  <input type="number" step="0.1" onChange={this.updateEchoFeedback} />
                </label>
                <label>
                  Wet Volume:
                  <input type="number" step="0.1" onChange={this.udpateEchoWetVolume} />
                </label>
                <label>
                  Dry Volume:
                  <input type="number" step="0.1" onChange={this.udpateEchoDryVolume} />
                </label>
              </div>
              <button onClick={this.sendPacket}>Update Audio</button>
            </>
        );
      }
}

export default UploadFile;