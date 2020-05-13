import React, { Component } from 'react';
import { Storage, API, graphqlOperation } from 'aws-amplify';
import * as queries from '../graphql/queries';
import * as mutations from '../graphql/mutations';

export default class ListFiles extends Component {
    constructor(props) {
        super(props);

        this.state = {
            fileList: ['No Files Uploaded'],
            fileToDelete: "No File"
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

        if(this.state.fileToDelete === "No File") {
            console.log("no file was specified");
        } else {
            // get list of all files in database
            const audioFiles = await API.graphql(graphqlOperation(queries.listAudios));
            const items = audioFiles.data.listAudios.items;
            var fileID = "No ID";

            // find id of file that needs to be deleted
            for (const i in items) {
                if(items[i].name == this.state.fileToDelete) {
                    fileID = items[i].id;
                }
            }

            if(fileID === "No ID") {
                console.log("file does not exist");
            } else {

                const audioDetails = {
                    id: fileID
                }
                
                // delete file from database
                const deletedFile = API.graphql(graphqlOperation(mutations.deleteAudio, {input: audioDetails}));

                // remove file from S3
                Storage.remove(this.state.fileToDelete)
                    .then(result => console.log(result))
                    .catch(err => console.log(err));

                this.setState({ fileToDelete: "No File" });
            }
        }   
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