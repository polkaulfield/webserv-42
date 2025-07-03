/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   uploadedFile.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arcebria <arcebria@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/01 17:51:48 by arcebria          #+#    #+#             */
/*   Updated: 2025/07/03 21:17:09 by arcebria         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UPLOADEDFILE_HPP
#define UPLOADEDFILE_HPP

#include <string>
#include <vector>
#include <fstream>
#include "clientRequest.hpp"
#include "serverResponse.hpp"
#include "../include/utils.hpp"

struct UploadedFile {
	std::string _filename;
	std::string _fieldName;
	std::string _contentType;
	std::vector<char> _content;
	size_t _size;

	UploadedFile() : _size(0) {}
};

#endif
