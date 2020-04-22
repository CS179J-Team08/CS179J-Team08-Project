import React, { Component } from 'react';
import './App.css';
import Amplify from 'aws-amplify';
import aws_exports from './aws-exports';
import { withAuthenticator, S3Image } from 'aws-amplify-react';
import { Connect } from 'aws-amplify-react';
import { API, graphqlOperation, Storage } from 'aws-amplify';
import { Form, Grid, Header, Input, List, Segment } from 'semantic-ui-react';
import { BrowserRouter as Router, Route, NavLink } from 'react-router-dom';
import { v4 as uuid } from 'uuid';

Amplify.configure(aws_exports);

class App extends Component {
  uploadFile = (evt) => {
    const file = evt.target.files[0];
    const name = file.name;

    Storage.put(name, file).then(() => {
      this.setState({ file: name });
    })
  }

  render() {
    return (
     <div className="App">
      <p>Upload a file</p>
      <input type="file" onChange={this.uploadFile} />
     </div>
    );
  }
}

export default withAuthenticator(App, { includeGreetings: true });
