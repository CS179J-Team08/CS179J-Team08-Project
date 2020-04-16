import React, { Component } from 'react';
import logo from './logo.svg';
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

class S3AudioUpload extends React.Component {
  constructor(props) {
    super(props);
    this.state = { uploading: false }
  }

  onChange = async (e) => {
    const file = e.target.files[0];
    const fileName = uuid();

    this.setState({ uploading: true });

    const result = await Storage.put(
      fileName,
      file,
      {
        customPrefix: { public: 'uploads/' },
        metadata: { dumpid: this.props.dumpId }
      }
    );

    console.log('Uploaded file: ', result);
    this.setState({ uploading: false });
  }

  render() {
    return(
      <div>
        <Form.Button 
          onClick={ () => document.getElementById('add-audio-file-input').click() } 
          disabled={ this.state.uploading }
          icon='file audio outline'
          content={ this.state.uploading ? 'Uploading...' : 'Add Audio'}
        />
        <input  
          id='add-audio-file-input'
          type="file"
          accept='audio/*'
          onChange={this.onChange}
          style={{ display: 'none' }}
        />
      </div>
    );
  }
}

const GetDump = `query GetDump($id: ID!) {
  getDump(id: $id) {
    id
    name
  }
}
`;

class DumpDetailsLoader extends React.Component {
  render() {
    return (
      <Connect query={graphqlOperation(GetDump, { id: this.props.id })}>
        {({ data, loading, errors }) => {
          if (loading) { 
            return <div>Loading...</div>; 
          }
          if (errors.length > 0) { 
            return <div>{JSON.stringify(errors)}</div>; 
          }
          if (!data.getDump) {
            return <div>No data</div>;          
          }
          
          return <DumpDetails dump={data.getDump} />;
        }}
      </Connect>
    );
  }
}

class DumpDetails extends Component {
  render() {
    return (
      <Segment>
        <h3>{this.props.dump.name}</h3>
        <S3AudioUpload dumpId={ this.props.dump.id } />
        <p>TODO: show files for this dump</p>
      </Segment>
    );
  }
}

class NewDump extends Component {
  constructor(props) {
    super(props);

    this.state = {
      dumpName: ''
    };
  }

  handleChange = (event) => {
    let change = {};
    change[event.target.name] = event.target.value;
    this.setState(change);
  }

  handleSubmit = async (event) => {
    event.preventDefault();
    const NewDump = `mutation NewDump($name: String!) { createDump(input: {name: $name}) { id name } }`;
    const result = await API.graphql(graphqlOperation(NewDump, { name: this.state.dumpName }));
    console.info('Created dump with id ${result.data.createDump.id}');
  }

  render() {
    return (
      <Segment>
        <h3>Create Dump</h3>
        <Input 
          type='text'
          placeholder='New Dump Name'
          icon='plus'
          iconPosition='left'
          action={{ content: 'Create', onClick: this.handleSubmit }}
          name='dumpName'
          value={this.state.dumpName}
          onChange={this.handleChange}
        />
      </Segment>
    );
  }
}

function makeComparator(key, order='asc') {
  return (a, b) => {
    if(!a.hasOwnProperty(key) || !b.hasOwnProperty(key)) {
      return 0;
    }

    const aVal = (typeof a[key] === 'string') ? a[key].toUpperCase() : a[key];
    const bVal = (typeof b[key] === 'string') ? b[key].toUpperCase() : b[key];

    let comparison = 0;
    if(aVal > bVal) {
      comparison = 1;
    }

    if(aVal < bVal) {
      comparison = -1;
    }

    return order === 'desc' ? (comparison * -1) : comparison
  };
}

class DumpsList extends React.Component {
  dumpItems() {
    return this.props.dumps.sort(makeComparator('name')).map(dump => 
      <List.Item key={dump.id}>
        <NavLink to={'/dumps/${dump.id}'}>{dump.name}</NavLink>
      </List.Item>
    );
  }

  render() {
    return (
      <Segment>
        <h3>My Dumps</h3>
        <List divided relaxes>
          {this.dumpItems()}
        </List>
      </Segment>
    );
  }
}

// query all dumps
const ListDumps = `query ListDumps {
   listDumps(limit: 9999) { 
     items {
      id 
      name 
      } 
    } 
  }`;

const SubscribeToNewDumps = `subscription OnCreateDump { 
  onCreateDump { 
    id
    name 
  } 
}`;

class DumpsListLoader extends React.Component {
  onNewDump = (prevQuery, newData) => {
    let updatedQuery = Object.assign({}, prevQuery);
    updatedQuery.listDumps.items = prevQuery.listDumps.items.concat([newData.onCreateDump]);
    return updatedQuery;
  }
  

  render() {
    return (
      <Connect 
          query={graphqlOperation(ListDumps)}
          subscription={graphqlOperation(SubscribeToNewDumps)} 
          onSubscriptionMsg={this.onNewDump}
      >
          
          {({ data, loading, errors }) => {
              if (loading) { return <div>Loading...</div>; }
              if (errors.length > 0) { return <div>{JSON.stringify(errors)}</div>; }
              if (!data.listDumps) return;

          return <DumpsList dumps={data.listDumps.items} />;
          }}
      </Connect>
    );
  }
}

class App extends Component {
  constructor(props) {
    super(props);
  }

  render() {
    return (
      <Router>
        <Grid padded>
          <Grid.Column>
            <Route path="/" exact component={NewDump} />
            <Route path="/" exact component={DumpsListLoader} />
            
            <Route path="/dumps/:dumpId" render = {
              () => <div><NavLink to='/'>Back to Dump List</NavLink></div>
            } />

            <Route path="/dumps/:dumpId" render = {
              props => <DumpDetailsLoader id={props.match.params.dumpId} />
            } />
          </Grid.Column>
        </Grid>
      </Router>
    );
  }
}

export default withAuthenticator(App, { includeGreetings: true });
