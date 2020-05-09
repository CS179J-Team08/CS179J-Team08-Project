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
        const audioFiles = await API.graphql(graphqlOperation(queries.listAudios));
        const items = audioFiles.data.listAudios.items;

        console.log(this.state.fileList);
        for (const i in items) {
            this.state.fileList[i] = items[i].name;
        }

        this.setState(this.state);
    }

    componentDidMount() {
        
    }
/*
    getUserAudioFiles = async () => {
        const audioFiles = await API.graphql(graphqlOperation(queries.listAudios));
        const items = audioFiles.data.listAudios.items;
        const filenames = []

        for (const i in items) {
            filenames[i] = items[i].name;
            console.log(filenames[i]);
        }
    }
*/
    render() {
        return(
            <div>
                <h3>File List</h3>
                <button onClick={ this.updateFileList }>Update File List</button>
                <ul>
                    { this.state.fileList.map(listFile => (
                        <li> { listFile } </li>
                    )) }
                </ul>
            </div>
        );
    }
}