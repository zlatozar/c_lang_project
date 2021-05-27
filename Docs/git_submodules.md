# Git Submodules basic explanation

_Submodule is a pointer to external module_.

### Why submodules?

In Git you can add a submodule to a repository. This is basically a
repository embedded in your **main** repository. This can be very
useful. A couple of advantages of using submodules:

- **You can separate the code into different repositories.**

    Useful if you have a codebase with big components, you could make a
    component a submodule. This way you'll have a cleaner Git log
    (commits are specific to a certain component).

- **You can add the submodule to multiple repositories.**

    Useful if you have multiple repositories that share the same
    components. With this approach you can easily update those
    components in all the repositories that added them as a submodule.
    This is a lot more convienient than copy-pasting the code into the
    repositories.


### Basics

When you add a submodule in Git, you don't add the **code** of the
submodule to the main repository, you only add **information about the
submodule** that is added to the main repository. This information
describes which **commit the submodule is pointing** at. This way, the
submodule's **code** won't automatically be updated if the submodule's
**repository** is updated. This is good, because your code might not
work with the latest commit of the submodule, it prevents unexpected
behaviour.


### Adding a submodule

You can add a submodule to a repository like this:

    git submodule add git@github.com:url_to/awesome_submodule.git path_to_awesome_submodule

With default configuration, this will check out the **code** of the
`awesome_submodule.git` repository to the `path_to_awesome_submodule`
directory, and will **add information to the main repository** about
this submodule, which contains the **commit the submodule points to**,
which will be the **current** commit of the default branch (usually the
`master` branch) at the time this command is executed.

After this operation, if you do a `git status` you'll see two files in
the `Changes to be committed` list: the `.gitmodules` file and the path
to the submodule. When you commit and push these files you commit/push
the submodule to the origin.

For example:

    git submodule add https://github.com/silentbicycle/greatest src/ext-libs/greatest/include


### Getting the submodule's code

If a new submodule is created by one person, the other people in the
team need to initiate this submodule. First you have to get the
**information** about the submodule, this is retrieved by a normal
`git pull`. If there are new submodules you'll see it in the output of
`git pull`. Then you'll have to initiate them with:

    git submodule init

This will pull all the **code** from the submodule and place it in the
directory that it's configured to.

If you've cloned a repository that makes use of submodules, you should
also run this command to get the submodule's code. This is not
automatically done by `git clone`.


### Pushing updates in the submodule

The submodule is just a separate repository. If you want to make changes
to it, you should make the changes in this repository and push them like
in a regular Git repository (just execute the git commands in the
submodule's directory). However, you should also let the **main**
repository know that you've updated the submodule's repository, and make
it use the latest commit of the repository of the submodule. Because if
you make new commits inside a submodule, the **main** repository will
still **point to the old commit**.

So, if you want to have these changes in your **main** repository too, you
should tell the **main** repository to use the latest commit of the
submodule. Now how do you do this?

So you've made changes in the submodule's repository and committed them
in its repository. If you now do a `git status` in the **main**
repository, you'll see that the submodule is in the list `Changes not
staged for commit` and it has the text `(modified content)` behind it.
This means that the **code** of the submodule is checked out on a
different commit than the **main** repository is **pointing to**. To
make the **main** repository **point to** this new commit, you just add
this change with `git add` and then commit and push it.


### Keeping your submodules up-to-date

If someone updated a submodule, the other team-members should update
the code of their submodules. This is not automatically done by
`git pull`, because with `git pull` it only retrieves the
**information** that the submodule is **pointing** to another
**commit**, but doesn't update the submodule's **code**. To update the
**code** of your submodules, you should run:

    git submodule update

#### What happens if you don't run this command?

If you don't run this command, the **code** of your submodule is checked
out to an **old** commit. When you do `git status` you will see the
submodule in the `Changes not staged for commit` list with the text
`(modified content)` behind it. This is not because you changed the
submodule's code, but because its **code** is checked out to a different
**commit**. So Git sees this as a change, but actually you
just didn't update the submodule's code. So if you're working with
submodules, don't forget to keep your submodules up-to-date.


### Making it easier for everyone

It is sometimes annoying if you forget to initiate and update your
submodules. Fortunately, there are some tricks to make it easier:

    git submodule update --init

This will update the submodules, and if they're not initiated yet, will
initiate them.

You can also have submodules **inside** of submodules. In this case you'll
want to update/initiate the submodules recursively:

    git submodule update --init --recursive

This is a lot to type, so you can make an alias:

    git config --global alias.update '!git pull && git submodule update --init --recursive'

Now whenever you execute `git update`, it will execute a `git pull` and
a `git submodule update --init --recursive`, thus updating all the code
in your project.
