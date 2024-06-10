'use strict';

describe('Service: digestAuth', function () {

  // load the service's module
  beforeEach(module('dashboardApp'));

  // instantiate service
  var digestAuth;
  beforeEach(inject(function (_digestAuth_) {
    digestAuth = _digestAuth_;
  }));

  it('should do something', function () {
    expect(!!digestAuth).toBe(true);
  });

});
