Feature: Routing and params processing

Scenario: Simple routing
  When I request "/"
  Then I should get "It works!"

Scenario: Serving static file
  When I request "/cosmonaut.png"
  Then I should get request with status "200"
   And its content type should be set to "image/png"

Scenario: Trying to access non-existent url
  When I request "/hey/you"
  Then I should get request with status "404"
   And I should get "Not Found"

Scenario Outline: Requesting action mounted to '/photos/(:id)'
  When I request "<path>"
   And I should get "<expected_response>"

  Examples:
    | path            | expected_response |
    | /photos/1       | [id:1]            |
    | /photos/word    | [id:word]         |
    | /photos/1/somth | Not Found         |
    | /photos/1/      | Not Found         |
    | /photos1/1      | Not Found         |

Scenario Outline: Requesting action mounted to '/patients/(:id)/(:action)'
  When I request "<path>"
   And I should get "<expected_response>"

  Examples:
    | path             | expected_response   |
    | /patients/1/edit | [id:1][action:edit] |
    | /patients/1/new  | [id:1][action:new]  |
    | /patients/1/     | [id:1][action:]     |
    | /patients//      | [id:][action:]      |

Scenario Outline: Requesting action mounted to '/gallery/(:gallery_id)/photo/(:photo_id)'
  When I request "<path>"
   And I should get "<expected_response>"

  Examples:
    | path                                | expected_response                           |
    | /gallery/1/photo/2                  | [gallery_id:1][photo_id:2]                  |
    | /gallery/las_vegas/photo/best_photo | [gallery_id:las_vegas][photo_id:best_photo] |
    | /gallery/gallery/photo/photo        | [gallery_id:gallery][photo_id:photo]        |
    | /gallery//photo/2                   | [gallery_id:][photo_id:2]                   |
    | /gallery/1/photo/                   | [gallery_id:1][photo_id:]                   |
    | /gallery/photo/2                    | Not Found                                   |
    | /gallery/1/photo                    | Not Found                                   |
    | /gall1ry/1/photo/2                  | Not Found                                   |

