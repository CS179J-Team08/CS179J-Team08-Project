import React, { Component } from 'react';
//import { Button } from 'react-bootstrap';
import './App.css';
import Amplify from 'aws-amplify';
import aws_exports from './aws-exports';
import config from './aws-exports';
import { withAuthenticator, S3Image } from 'aws-amplify-react';
import { API, graphqlOperation, Storage } from 'aws-amplify';
import { Form, Grid, Header, Input, List, Segment } from 'semantic-ui-react';
import { v4 as uuid } from 'uuid';

import UploadFile from "./Components/UploadFile";
import ListFiles from "./Components/ListFiles";

Amplify.configure(aws_exports);

class App extends Component {
  render() {
    return (
     <div className="App">
       <UploadFile />
       <ListFiles />
     </div>
    );
  }
}

export default withAuthenticator(App, true);
