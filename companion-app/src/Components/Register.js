import React, { Component } from 'react';
import { Auth, API, Storage } from 'aws-amplify';
let devicePacket = {
    group: [
      { userID: "TutorialTestQueue" },
      { userID: "" },
      { userID: "" },
      { userID: "" },
      { userID: "" },
    ],
    device: "",
    queue: ""
};

export default class App extends Component {
    constructor(props) {   
        super(props);
        this.state = {
            device : ""
        }
        this.addDevice = this.addDevice.bind(this);
    }

    addDevice = async () =>   {
        devicePacket.device = this.state.device;
        let user = await Auth.currentAuthenticatedUser();
        const { attributes } = user;
        devicePacket.queue = attributes.sub;
        console.log(devicePacket);
        console.log(devicePacket.queue);
        let fileName = "register.json";
        Storage.put(fileName, devicePacket)
        .then(result => console.log(result))
        .catch(err => console.log(err));



    }
    render() {
        return (
            <>
               <div>
                  <h3>Device Menu</h3>
                  <p>Register your device below</p>
                   <label>
                      Device #
                      <input type="text" onChange={ (event) => {
                          let deviceName = this.state.device;
                          deviceName = "device-" + event.target.value;
                          this.setState({
                              device: deviceName
                          });
                      } } />
                   </label>
                   <button onClick={this.addDevice}>Send Registration</button>
               </div>
            </>
        );
    }
}
