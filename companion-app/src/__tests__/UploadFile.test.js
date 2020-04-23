import React from 'react';
import ReactTestUtils from 'react-dom/test-utils'; 
import UploadFile from "../Components/UploadFile";
import renderer from 'react-test-renderer';


 describe('UploadFile', () => {
 
    it('has a p tag', () => {
      const component = ReactTestUtils.renderIntoDocument(<UploadFile />);
      var p = ReactTestUtils.findRenderedDOMComponentWithTag(
        component, 'p'
      );
    });

    it('allows input', () => {
      const component = ReactTestUtils.renderIntoDocument(<UploadFile />);
      var inp = ReactTestUtils.findRenderedDOMComponentWithTag(
        component, 'input'
      );

    });

  it('renders a snapshot', () => {
    const tree = renderer.create(<UploadFile />).toJSON();
    expect(tree).toMatchSnapshot();
  })
})
