import React, { Component } from 'react';
import { Storage, API, graphqlOperation } from 'aws-amplify';
import * as queries from '../graphql/queries';
import * as mutations from '../graphql/mutations';

export default class ListFiles extends Component {
    constructor(props) {
        super(props);

        this.state = {
            fileList: ['No Files Uploaded'],
            fileToDelete: ""
        }

        this.updateFileList = this.updateFileList.bind(this);
    }

    updateFileList = async () => {
        // fetch files from database
        const audioFiles = await API.graphql(graphqlOperation(queries.listAudios));
        const items = audioFiles.data.listAudios.items;

        // update file list
        for (const i in items) {
            this.state.fileList[i] = items[i].name;
        }

        // re render (there is probably way better ways to do this)
        this.setState(this.state);
    }

    deleteFile = async (event) => {
        event.preventDefault();

        const audioFiles = await API.graphql(graphqlOperation(queries.listAudios));
        const items = audioFiles.data.listAudios.items;
        var fileID;

        for (const i in items) {
            if(items[i].name == this.state.fileToDelete) {
                fileID = items[i].id;
            }
        }

        const audioDetails = {
            id: fileID
        }
        
        const deletedFile = API.graphql(graphqlOperation(mutations.deleteAudio, {input: audioDetails}));

        Storage.remove(this.state.fileToDelete)
            .then(result => console.log(result))
            .catch(err => console.log(err));
    }

    render() {
        return(
            <div>
                <h2>File List</h2>
                <form onSubmit={this.deleteFile}>
                    <label>File to delete
                        <input type="text" onChange={ (event) => {
                            this.setState({
                                fileToDelete: event.target.value
                            });
                        }} />
                    </label>
                    <input type="submit" value="Delete" />
                </form>
                <p></p>
                <button onClick={ this.updateFileList }>Refresh File List</button>
                <ul>
                    { this.state.fileList.map(listFile => (
                        <li> { listFile } </li>
                    )) }
                </ul>
            </div>
        );
    }
}