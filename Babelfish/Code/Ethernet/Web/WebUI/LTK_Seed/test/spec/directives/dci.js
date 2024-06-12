'use strict';

describe('Directive: dci', function () {

  // load the directive's module
  beforeEach(module('lecamApp'));

  var element,
    scope;

  beforeEach(inject(function ($rootScope) {
    scope = $rootScope.$new();
  }));

  it('should make hidden element visible', inject(function ($compile) {
    element = angular.element('<dci></dci>');
    element = $compile(element)(scope);
    expect(element.text()).toBe('this is the dci directive');
  }));
});
