import React, { Component } from 'react';
import './App.css';
import Amplify from 'aws-amplify';
import aws_exports from './aws-exports';
import { withAuthenticator } from 'aws-amplify-react';

import { BrowserRouter as Router, Route, Link } from 'react-router-dom';

import UploadFile from "./Components/UploadFile";
import ListFiles from "./Components/ListFiles";
import GroupPlay from "./Components/GroupPlay"

Amplify.configure(aws_exports);

class App extends Component {
  render() {
    return (
      <>
        <Router>
          <Link to="/"> Home </Link>
          <Link to="/solo-play"> Solo Play </Link>  
          <Link to="/group-play"> Group Play </Link>  
          <Route exact path="/solo-play" render={ props =>
            <div>
              <UploadFile />
              <ListFiles /> 
            </div> 
          } />
          <Route exact path="/group-play" render={ props =>
              <div>
                <GroupPlay />
              </div>
            } />
        </Router>
      </>
    );
  }
}

export default withAuthenticator(App, true);
 