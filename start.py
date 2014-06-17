#
# Copyright 2014 Yong-Siang Shih.
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License version 3,
# as published by the Free Software Foundation.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

import argparse
import json
import os

import sqlite3
from flask import Flask, request, abort, render_template
from flask.json import jsonify

import cirb_search.search
import search
import semantic

app = Flask(__name__)
app.config.from_object(__name__)
app.config.update(dict(
    DEBUG=True,
    ))
app.config.from_envvar('FLASKR_SETTINGS', silent=True)

@app.route('/')
def show_entries():
    return render_template('index.html')

def ajax_search():
    data = request.form.get('data', '', type=str)
    if data != '':
        if app.config['search_method'] == 'google':
            events = search.use_google_news(data)
        elif app.config['search_method'] == 'cirb':
            events = search.use_cirb(data, cirb_eng)
        else:
            events = []
        results = semantic.score_semantic(events, score_judger)
        return jsonify(items=results)
    abort(404)

@app.route('/ajax', methods=['GET', 'POST'])
def ajax():
    fnc = request.form.get('fnc', '', type=str)
    if fnc == 'search':
        return ajax_search()
    abort(404)

def process_commands():
    parser = argparse.ArgumentParser(description='Start NewsMeter server.')

    parser.add_argument('-s', dest='search_method', default='google',
            choices=('google', 'cirb'))
    parser.add_argument('-p', dest='port', default=5000, type=int)

    # options for cirb
    group = parser.add_argument_group('cirb')
    group.add_argument('-r', dest='rel_feedback', action='store_true',
            help='If specified, turn on the relevance feedback.')
    group.add_argument('-d', dest='news_dir', metavar='news-dir',
            help='The directory of NEWS documents. '
            'ex. If the directory\'s pathname is /tmp2/CIRB010, '
            'it will be "-d /tmp2/CIRB010".')
    group.add_argument('-m', dest='wm_dir', metavar='wm-dir',
            help='The input wm directory, which includes the files: '
            'vocab.all, file-list, inverted-index, stop-list, doc-length, score-list')
    group.add_argument('-l', dest='db_path', metavar='DB-PATH',
            help='Load SQLite3 database.')

    args = parser.parse_args()

    if args.search_method == 'cirb':
        if not (args.news_dir and args.db_path and args.wm_dir):
            parser.error('need to specify cirb arguments')

    return args

if __name__ == '__main__':
    global cirb_eng
    args = process_commands()
    app.config['search_method'] = args.search_method
    if args.search_method == 'cirb':
        cirb_eng = cirb_search.search.Search(
                os.path.abspath(args.wm_dir),
                os.path.abspath(args.news_dir),
                os.path.abspath(args.db_path),
                args.rel_feedback)
        score_judger = semantic.Judger(args.wm_dir, os.path.join(args.wm_dir, 'score-list'))
    else:
        score_judger = semantic.RandomJudger()
    app.run(port=args.port)
