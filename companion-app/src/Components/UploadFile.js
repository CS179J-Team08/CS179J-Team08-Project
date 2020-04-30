import React, { useEffect, useState, Component } from 'react';
import { Storage, API, graphqlOperation } from 'aws-amplify'
import uuid from 'uuid/v4'
import { withAuthenticator } from 'aws-amplify-react'

class UploadFile extends Component {
  /*
    uploadFile = (evt) => {
        const file = evt.target.files[0];
        const name = file.name;
    
        Storage.put(name, file, { level: 'private' }).then(() => {
          this.setState({ file: name });
        })
      }
  */

      render() {
        return (
            <div className="App">
             <p>Upload a file</p>
             <input type="file" onChange={this.uploadFile} />
            </div>
           );
      }
}

export default UploadFile;