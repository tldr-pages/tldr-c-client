# Contributing

Contributions through [pull requests]
are most welcome!

Please read the whole document before contributing, to minimize the chances of
your change being rejected.


# Guidelines

Please follow these few guidelines, to make sure your pull request will be accepted:

- read and follow the [coding standard]
- add a test for the features added
- make sure it builds and all tests pass
- if it's a major change, or fixes a high priority bug, add a new change to the changelog
- squash the commits to the required minimum, and name them correctly
- keep your commits small and contain one task or one fix (keep it "atomic")


# Choosing the right branch

All pull requests have to be filed against the `master` branch, since this is
where all of the development is happening.
Pull requests to other branches will not be accepted.


# How to name your commits

1. Separate subject from body with a blank line
2. Limit the subject line to 50 characters
3. Capitalize the subject line
4. Do not end the subject line with a period
5. Use the imperative mood in the subject line
6. Wrap the body at 72 characters
7. Use the body to explain *what* and *why* vs. *how*

Complete example:

    Summarize changes in around 50 characters or less

    More detailed explanatory text, if necessary. Wrap it to about 72
    characters or so. In some contexts, the first line is treated as the
    subject of the commit and the rest of the text as the body. The
    blank line separating the summary from the body is critical (unless
    you omit the body entirely); various tools like `log`, `shortlog`
    and `rebase` can get confused if you run the two together.

    Explain the problem that this commit is solving. Focus on why you
    are making this change as opposed to how (the code explains that).
    Are there side effects or other unintuitive consequences of this
    change? Here's the place to explain them.

    Further paragraphs come after blank lines.

     - Bullet points are okay, too

     - A hyphen is used for the bullet, preceded by a single space, with blank
       lines in between, and continuations are done on the same paragraph as the
       beginning

    Put references to issues resolved or related at the bottom:

    Resolves: #123
    See also: #456, #789


# Licensing

My submitting a patch you agree to license your patch under the MIT License.


[pull requests]:    https://help.github.com/articles/using-pull-requests/
[coding standard]:  https://github.com/tldr-pages/tldr-c-client/blob/master/STYLE.md

