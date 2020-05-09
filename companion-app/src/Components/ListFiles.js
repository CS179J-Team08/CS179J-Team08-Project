import React, { useEffect, useState, Component } from 'react';
import { API, graphqlOperation } from 'aws-amplify';
import * as queries from '../graphql/queries';
import { ListItem } from 'semantic-ui-react';

export default class ListFiles extends Component {
    constructor(props) {
        super(props);

        this.state = {
            fileList: ['No Files Uploaded']
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

    componentDidMount() {
        
    }

    render() {
        return(
            <div>
                <h3>File List</h3>
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